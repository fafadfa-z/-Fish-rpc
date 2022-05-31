#include "registry/rpc_registry.h"
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

        TcpServer::setBeginCallBack([this](Channel::ptr channel)
                                   { sendMes(channel); });

        TcpServer::begin();

        TcpServer::createConnection({"127.0.0.1",8848});
    }

    void RpcRegistry::handleMessage(Channel::ptr channel)
    {
    }


    #include <iostream>

    using namespace std;

    void RpcRegistry::sendMes(Channel::ptr channel)
    {
        auto protocol = Protocol::createHealthPacket(1);

        protocol->printMes();

        auto mes = protocol->result();

        channel->send({mes.c_str(),mes.size()});

        // std::string str;

        // str.assign("123");

        // channel->send(str);

    }

}
