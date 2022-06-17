#include "net/uring_io.h"
#include "net/channel.h"
#include "net/tcp_server.h"

#include <memory>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <string_view>

    using namespace std;

namespace Fish
{
    Uring::Uring(int queue_size)
        : freeNum_(queue_size - 1), queue_size_(queue_size)
    {
        io_uring_params params;
        ::memset(&params, 0, sizeof(params));

        auto ret = io_uring_queue_init_params(queue_size, &ring_, &params);

        assert(ret == 0);

        ret = params.features & IORING_FEAT_FAST_POLL;

        assert(ret != 0);

        io_uring_probe *probe;

        probe = io_uring_get_probe_ring(&ring_);

        if (!probe || !io_uring_opcode_supported(probe, IORING_OP_PROVIDE_BUFFERS))
        {
            std::terminate();
        }

        free(probe);

        loop_.setTask([this]
                      { eventLoop(); });
    }

    Uring::~Uring()
    {
        io_uring_queue_exit(&ring_);
    }

    void Uring::beginLoop()
    {
        loop_.setTask([this]
                      { eventLoop(); });

        auto thread = loop_.beginInNewThread();

        thread.detach();
    }

    // 为每一个连接开一个协程

    Task Uring::coroutineFun(Channel &channel)
    {
        // std::cout << "coroutine begin" << std::endl;

        while (true)
        {
            auto view = co_await channel.co_read();

            if (view.size() == 0)
            {
                channel.close();
                co_return; //以后可能会加入统计信息
            }

            channel(); //回调服务器的函数
        }
    }

    void Uring::eventLoop() //在循环中跑的函数
    {
        io_uring_cqe *cqe;

        int index;
        int count = 0;

        assert(freeNum_ >= 0);

        io_uring_for_each_cqe(&ring_, index, cqe)
        {
            count++;

            UringRequest *flag = (UringRequest *)io_uring_cqe_get_data(cqe);

            task_option type = (task_option)flag->event_type;
            int fd = flag->client_socket;
            Channel *channel = flag->channel;

            delete flag;

            // int fd = quest.client_socket;

            if (type == READ)
            {

                auto iter = connections_.find(fd);

                assert(iter != connections_.end());

                auto res = cqe->res;

                if (res <= 0 and errno == 0)  //连接断开
                {
                    channel->close();
                    freeNum_++;
                    continue;
                }

                channel->alreadyRead(res);
                channel->task().handler.resume();

                freeNum_++;
            }

            else if (type == WRITE)
            {
                auto iter = connections_.find(fd);

                assert(iter != connections_.end());

                auto channel = iter->second;

                auto res = cqe->res;
        
                channel ->eraseWrite(res);

                auto sendView = channel->dispSendBuf();

                if(sendView.size()>0) //在发送过程中有额外的数据到来，还需发送
                {
                    addWriteRequest(channel->fd(),sendView.data(),sendView.size());
                }
                else //没有要发送的数据了
                { 
                    channel->reduseSend();
                    freeNum_++;
                }


            }
        }

        io_uring_cq_advance(&ring_, count);

        beginRecv(); //提交新的接收任务

        beginSend(); //提交新的发送任务

        io_uring_submit(&ring_);
    }

    void Uring::beginRecv()
    {
        std::queue<int> tempRecv;
        {
            MutexType::guard guard(recvQueMut_);

            auto size = recvQue_.size();

            if (size == 0)
                return;

            if (size <= freeNum_)
            {
                freeNum_ -= size;
                std::swap(tempRecv, recvQue_);
            }
            else
            {
                for (size_t i = 0; i < freeNum_; i++)
                {
                    tempRecv.push(recvQue_.front());
                    recvQue_.pop();
                }
                freeNum_ = 0;
            }
        }

        while (!tempRecv.empty()) //新fd, 开启接收
        {
            auto fd = tempRecv.front();
            tempRecv.pop();

            ::io_uring_sqe *sqe = io_uring_get_sqe(&ring_);

            auto iter = connections_.find(fd);

            assert(iter != connections_.end());

            auto &channel = iter->second;

            assert(channel->fd() == fd);

            UringRequest *flag = new UringRequest;

            flag->event_type = READ;
            flag->client_socket = fd;
            flag->channel = channel.get();
            // sqe->user_data = flag.uring_data;

            auto buf = channel->dispWriteBuf();

            io_uring_prep_recv(sqe, fd, const_cast<char*>(buf.data()), buf.size(), 0);

            io_uring_sqe_set_data(sqe, flag);
        }
    }


    void Uring::beginSend()
    {
        // cout<<"---------------------"<<endl;
        std::queue<std::tuple<int, const char *, int>> tempSend;
        {
            MutexType::guard guard(sendQueMut_);

            auto size = sendQue_.size();

            if (size == 0)
                return;

            if (size <= freeNum_)
            {
                freeNum_ -= size;
                std::swap(tempSend, sendQue_);
            }
            else
            {
                for (size_t i = 0; i < freeNum_; i++)
                {

                    tempSend.push(sendQue_.front());
                    sendQue_.pop();
                }
                freeNum_ = 0;
            }
        }

        while (!tempSend.empty())
        {
            auto conn = tempSend.front();
            tempSend.pop();

            auto [fd, ptr, len] = conn;
            auto iter = connections_.find(fd);

            if(iter == connections_.end()) // 连接已经被删除
            {
                continue;
            }
            ::io_uring_sqe *sqe = io_uring_get_sqe(&ring_);

            Channel &channel = *(iter->second).get();

            UringRequest *flag = new UringRequest;

            flag->event_type = WRITE;
            flag->client_socket = fd;
            flag->channel = &channel;

            io_uring_prep_send(sqe, fd, ptr, len, 0);

            io_uring_sqe_set_data(sqe, flag);
        }
    }

    int Uring::addWriteRequest(int fd, const char *buf, int size)
    {
        MutexType::guard guard(sendQueMut_);

        sendQue_.push({fd, buf, size});

        return 0;
    }

    int Uring::addReadRequest(int fd)
    {
        MutexType::guard guard(recvQueMut_);
        recvQue_.push(fd);

        return recvQue_.size();
    }

    void Uring::removeFd(Channel::ptr channel)
    {
        auto fd = channel->fd();

        auto iter = connections_.find(fd);

        assert(iter != connections_.end());

        assert(iter->second == channel);

        connections_.erase(iter);

        std::cout << "close socket: " << fd << std::endl;

        ::close(fd);
    }

    Channel::ptr Uring::addNewFd(int fd)
    {
        auto iter = connections_.find(fd);

        assert(iter == connections_.end());

        Channel::ptr newChannel = std::make_shared<Channel>(fd, this, 1024);

        newChannel->setCallBack(server_->readCallBack_);
        newChannel->setCloseCallBack(server_->closeCallBack_);

        connections_.insert({fd, newChannel});

        auto task  = coroutineFun(*newChannel);

        newChannel->setTask(task); //开启协程;

        return newChannel;
    }

}
