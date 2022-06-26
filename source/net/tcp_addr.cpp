#include "net/tcp_addr.h"


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

    TcpAddr::TcpAddr(const sockaddr_in& addr)
    {
        char ipBuf[INET_ADDRSTRLEN];
        
        auto ip = inet_ntop(AF_INET, &addr.sin_addr, ipBuf, INET_ADDRSTRLEN);
        auto port = ntohs(addr.sin_port);

        if (avlid(ip, port))
        {
            ip_ = ip;
            port_ = port;
            return;
        }

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
                ip_ = ip;
                port_ = port;
                return;
            }
        }
        
        throw std::runtime_error("TcpAddr: invalid ip or port");
    }

    std::string TcpAddr::toStr()
    {
        std::string result;

        result += "[";
        result += ip_;
        result += " : ";
        result += std::to_string(port_);
        result += "]";

        return result;
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
