#include "rpc/rpc_registry.h"
#include "net/channel.h"

#include "rpc/protocol.h"


namespace Fish
{
    RpcRegistry::RpcRegistry(TcpAddr addr, std::string name)
        : TcpServer(addr, name)
    {



    }

    void RpcRegistry::begin()
    {
        TcpServer::setReadCallBack([this](Channel::ptr channel)
                                   { handleMessage(channel); });
        TcpServer::begin();
    }

    void RpcRegistry::handleMessage(Channel::ptr channel)
    {

        
    }

}
