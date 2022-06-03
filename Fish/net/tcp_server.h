#pragma once

#include <memory>
#include <atomic>
#include <string>
#include <functional>

#include <arpa/inet.h>

#include "base/non_copyable.h"
#include "base/loop.h"

#include "tcp_addr.h"
#include "uring_io.h"


namespace Fish
{
    class Uring;
    class Channel;
    class Timer;

    class TcpServer : public NonCopyable
    {
    public:
        using ptr = std::shared_ptr<TcpServer>;
        using CallBackFun = std::function<void(std::shared_ptr<Channel>)>;

        TcpServer(TcpAddr, std::string = "");

        virtual ~TcpServer();

        //开始服务器监听
        virtual void begin();

        virtual void stop();

        void createConnection(const TcpAddr&);


        void setReadCallBack(CallBackFun cb){readCallBack_ = cb;}
        void setCloseCallBack(std::function<void(int)> cb){closeCallBack_ = cb;}
        void setNewCallBack(std::function<void(int)> cb){newCallBack_ = cb;}

        void setBeginCallBack(CallBackFun cb){beginCallBack_ = cb;}
        void setName(std::string name) { name_ = name; }
        void setTimer(Timer* timer){timer_ = timer;}


        const std::string &name() const { return name_; }
    protected:
        int createListenSocket();

        virtual void listen_in_loop();

        sockaddr_in clientAddr_ = {0};
        TcpAddr addr_;

        Timer * timer_;


    private:
        CallBackFun readCallBack_;   //收到信息回调
        std::function<void(int)> closeCallBack_; // 连接断开回调
        std::function<void(int)> newCallBack_; //新连接到来回调

        CallBackFun beginCallBack_;  // 发起连接后回调

        int listenFd_ = -1;
        friend class Uring;
        Uring uring_;


        std::string name_;

        Loop loop_;
    };

}
