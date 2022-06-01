#pragma once

#include <liburing.h>
#include <queue>
#include <map>
#include <memory>
#include <tuple>

#include "base/sync/mutex.h"
#include "base/loop.h"
#include "net/framework.h"


namespace Fish
{
    class Channel;
    class TcpServer;

    class Uring
    {
    public:
        using MutexType = Mutex;

        Uring(int queue_size = 1024);

        virtual ~Uring();

        void beginLoop();

        int addWriteRequest(int fd, const char *, int size);

        int addReadRequest(int fd);

        void removeFd(std::shared_ptr<Channel>);

        [[maybe_unused]]std::shared_ptr<Channel> addNewFd(int fd);
        

        void eventLoop();


        static void setServer(TcpServer* server){server_ = server;}


    private:
        std::queue<int> recvQue_; //储存待发送的文件描述符
        MutexType recvQueMut_;

        std::queue<std::tuple<int,const char*,int>> sendQue_; //储存发送的文件描述符
        MutexType sendQueMut_;

    private:
        std::map<int, std::shared_ptr<Channel>> connections_; //连接的协程

        static Task coroutineFun(Channel&);

        void beginRecv();
        void beginSend();

    private:
        io_uring ring_;

        Loop loop_;

        size_t freeNum_; //当前空闲的请求数
        size_t queue_size_; //请求的最大数

        constexpr static int buf_size_ = 2048;


        inline static TcpServer* server_;
    };
}
