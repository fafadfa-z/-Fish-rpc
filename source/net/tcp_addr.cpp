#include "net/tcp_addr.h"

#include <arpa/inet.h>
#include <stdexcept>

namespace Fish
{

    TcpAddr::TcpAddr(const std::string str, int port)
    {
        if (avlid(str, port))
        {
            ip_ = str;
            port_ = port;
        }
        else
            throw std::runtime_error("TcpAddr: invalid ip or port");
        
    }

    TcpAddr::TcpAddr(const std::string str) //输入格式为 "127.0.0.1:8848"
    {
        auto iter = std::find(str.begin(), str.end(), ':');

        if (iter != str.end())
        {
            auto ip = std::string(str.begin(), iter);

            auto port_str = std::string(iter + 1, str.end());
            auto port = std::atoi(port_str.c_str());

            if (avlid(ip, port))
            {
                ip_ = str;
                port_ = port;
                return;
            }
        }
        
        throw std::runtime_error("TcpAddr: invalid ip or port");
    }


    bool TcpAddr::avlid(const std::string &ip, const int port)
    {
        struct in_addr s;
        auto ret = inet_pton(AF_INET, ip.c_str(), (void *)&s);

        if (ret != 1)
            return false;

        if (port > 65536 or port <= 0)
            return false;

        return true;
    }

}
