#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "net/tcp_addr.h"

#include "net/tcp_server.h"

namespace Fish
{
    struct ProviderMes //用于储存 服务提供方的所有信息
    {
        using ptr = std::shared_ptr<ProviderMes>;

        std::string name;

        TcpAddr addr;
    };

    class Channel;

    class RpcRegistry : public TcpServer
    {
    public:
        RpcRegistry(TcpAddr addr, std::string name = "rpc register");

        void begin() override;

        void handleMessage(std::shared_ptr<Channel>);

        void sendMes(std::shared_ptr<Channel>);

        const std::string &name() { return name_; }

    private:

        
    
    
        std::string name_;
    };

}
