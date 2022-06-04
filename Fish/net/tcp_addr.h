#pragma once

#include <string>
#include <arpa/inet.h>

namespace Fish
{
    class TcpAddr
    {
    public:
        TcpAddr() = default;

        TcpAddr(const sockaddr_in&);

        TcpAddr(const std::string str, int port);
       

        TcpAddr(const std::string str);  //输入格式为 "127.0.0.1:8848"
 

        const std::string& ip()const{return ip_;}

        const int port() const{return port_;}


    private:

        bool avlid(const std::string&,const int);


        std::string ip_;

        int port_ = -1;
    };

}
