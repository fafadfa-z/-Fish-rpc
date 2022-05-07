#pragma once

#include <memory>
#include <atomic>
#include "base/non_copyable.h"

#include "tcp_server.h"
#include "tcp_addr.h"
#include "uring_io.h"

namespace Fish
{
    class Uring;

    class TcpServer :public NonCopyable
    {
    public:
        using ptr = std::shared_ptr<TcpServer>;

        TcpServer(TcpAddr);


        //开始服务器监听，此函数会阻塞当前程序流
        void begin();

    int createListenSocket();



    private:

        int listenFd_ = -1;

        std::atomic<bool> runningFlag_ =  false;
        
        Uring uring_;

        TcpAddr addr_;
    };

}
