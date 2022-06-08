#pragma once

#include <string>
#include <memory>
#include <vector>


#include "net/tcp_addr.h"
#include "base/sync/mutex.h"
#include "net/tcp_server.h"

#include "rpc/protocol_manager.h"
#include "rpc/external_node.h"


namespace Fish
{

    class Channel;

    class RpcRegistry : public TcpServer
    {
    public:
        RpcRegistry(TcpAddr addr, std::string name = "rpc register", uint16_t id = 0);

        void begin() override;

        void handleMessage(std::shared_ptr<Channel>);

        const std::string &name()const { return name_; }

    private:

        uint16_t id_;

 

        // 处理新连接的 provider
        void providerNew(Protocol::ptr &, std::shared_ptr<Channel> &);

        // 处理断开的 provider
        void providerErase(size_t id);


    private:
        ProtocolManager protocols_;
        NodeManager nodes_;
        

        std::string name_;
    };

}
