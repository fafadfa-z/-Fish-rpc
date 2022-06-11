#pragma once

#include "buffer.h"
#include "base/non_copyable.h"
#include "net/framework.h"
#include "net/uring_io.h"
#include "tcp_addr.h"
#include <functional>
#include <cassert>
#include <memory>
#include <atomic>

namespace Fish
{
    class Channel : public NonCopyable, public std::enable_shared_from_this<Channel>
    {
    public:
        using ptr = std::shared_ptr<Channel>;

        Channel(int, Uring *, int = 1024);

        int fd() const { return fd_; }
        auto uring() const { return uring_; }
        auto task() const { return task_; }
        auto addr() const { return addr_; }
        void send(std::string_view); //发送数据(提交发送申请)


    private:

        // 对tcpserver 开放所有接口
        friend class Uring;
        friend class TcpServer;
        friend class ReadWaitor;
        friend class ProtocolManager;
    
        //对外部屏蔽下面的接口
        void setAddr(const TcpAddr &addr) { addr_ = addr; }
        void setCallBack(std::function<void(Channel::ptr)> cb) { cb_ = cb; }
        void setTask(Task task){ task_ = task;}

        void setCloseCallBack(std::function<void(int)> cb) { closeCb_ = cb; }

        auto dispReadBuf() { return readBuf_.disRead(); }
        auto dispWriteBuf() { return readBuf_.disWrite(); }
        
        auto dispSendBuf() {return sendBuf_.disRead();}

        void clearReadBuf() { readBuf_.clear(); } //清空buf
        void clearSendBuf() { sendBuf_.clear(); } //清空发送buf

        auto eraseRead(size_t len) { readBuf_.eraseData(len); } //前面的数据已经被读取后标记为删除
        auto eraseWrite(size_t len) { sendBuf_.eraseData(len); }

        auto alreadyRead(size_t len) { readBuf_.already(len); } //由外部直接输入并添加到后面的数据
        auto alreadyWrite(size_t len) { sendBuf_.already(len); } 

        void reduseSend(); //有发送请求完成后调用

        void operator()()
        {
            assert(cb_);
            cb_(shared_from_this());
        }

        void close(); //通道关闭

        auto co_read() //由协程调用，返回awaitor
        {
            return ReadWaitor(*this);
        }

    private:
        Task task_;
        Buffer readBuf_;
        Buffer sendBuf_;
        TcpAddr addr_;
        Uring *uring_;

    private:
        std::atomic<int> sendFlag_ = 0; // 标志当前是否有send的数据
        bool closeFlag_ = false;        //延时删除的标记位

        std::function<void(Channel::ptr)> cb_;
        std::function<void(int)> closeCb_;

        int fd_;
    };
}
