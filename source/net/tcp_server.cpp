#include "net/tcp_server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cassert>
#include <string.h>

#include "base/log/logger.h"

namespace Fish
{
    TcpServer::TcpServer(TcpAddr addr, std::string name)
        : addr_(addr), name_(name),loop_("tcp servr loop")
    {
        createListenSocket();

        Uring::setServer(this);
    }

    int TcpServer::createListenSocket()
    {
        listenFd_ = socket(AF_INET, SOCK_STREAM, 0);

        assert(listenFd_ > 0);

        int temp = 1;
        ::setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &temp, sizeof(temp));

        struct sockaddr_in servaddr;
        ::memset(&servaddr, 0, sizeof(servaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        // servaddr.sin_addr.s_addr = inet_addr("192.168.190.134);
        servaddr.sin_port = htons(addr_.port());

        auto ret = bind(listenFd_, (struct sockaddr *)&servaddr, sizeof(servaddr));

        assert(ret == 0);

        return listenFd_;
    }

    void TcpServer::begin()
    {
        // 这里应该加一些检查

        LOG_COUT<<name_<<" begin listen...."<<std::endl;

        assert(listenFd_ != -1);

        listen(listenFd_, 64);

        uring_.beginLoop();

        loop_.setTask([this]{listen_in_loop();});

        loop_.beginInCurrentThread();
    }

    void TcpServer::listen_in_loop()  //监听、接收新连接
    {
        clientAddr_ = {0};

        socklen_t socklen = sizeof(clientAddr_);

        auto fd = ::accept(listenFd_, (struct sockaddr *)&clientAddr_,&socklen);

        assert(fd>0);

        if(newCallBack_) newCallBack_(fd);

        uring_.addNewFd(fd);
    }

    void TcpServer::stop()
    {
        loop_.closeLoop();
    }

    TcpServer::~TcpServer()
    {
        
    }

}
