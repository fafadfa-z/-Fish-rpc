#pragma once

#include <memory>
#include <atomic>
#include <string>

#include <arpa/inet.h>

#include "base/non_copyable.h"
#include "base/loop.h"

#include "tcp_addr.h"
#include "uring_io.h"

namespace Fish
{
    class Uring;

    class TcpServer : public NonCopyable
    {
    public:
        using ptr = std::shared_ptr<TcpServer>;

        TcpServer(TcpAddr, std::string = "");

        virtual ~TcpServer();

        //开始服务器监听
        virtual void begin();

        virtual void stop();

        int createListenSocket();

        const std::string &name() const { return name_; }

        void setName(std::string name) { name_ = name; }

    private:
        virtual void listen_in_loop();

        sockaddr_in clientAddr_ = {0};

    private:
        int listenFd_ = -1;

        Uring uring_;

        TcpAddr addr_;

        std::string name_;

        Loop loop_;
    };

}
