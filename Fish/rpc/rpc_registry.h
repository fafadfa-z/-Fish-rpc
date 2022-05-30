#pragma once

#include <string>
#include <memory>

#include "net/tcp_server.h"

namespace Fish
{
    class Channel;

    class RpcRegistry : public TcpServer
    {
        public:

        RpcRegistry(TcpAddr addr, std::string name = "rpc register");


        void begin();

        void handleMessage(std::shared_ptr<Channel>);

        const std::string& name(){return name_;}

        private:
        
        std::string name_;
    };

}
