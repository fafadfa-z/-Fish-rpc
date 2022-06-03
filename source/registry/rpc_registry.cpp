#include "registry/rpc_registry.h"
#include "net/channel.h"

#include "rpc/protocol.h"
#include "base/log/logger.h"

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

        // TcpServer::setBeginCallBack([this](Channel::ptr channel)
                                    // { sendMes(channel); });

        TcpServer::begin();

        // TcpServer::createConnection({"127.0.0.1", 8848});
    }

    void RpcRegistry::handleMessage(Channel::ptr channel)
    {
        auto protocol = protocols_.readMes(channel);

        if (!protocol)
            return;

        assert(protocol->process() == ProtocolProcess::perfect);

        auto type = protocol->type();

        switch (type)
        {
        case MsgType::Rpc_None:
            LOG_FATAL << "不应该出现的情况..." << Fish::end;
            break;

        case MsgType::Rpc_Health:
            // hanleHealth(protocol, channel);

            break;
        case MsgType::Rpc_Provider:

            break;
        case MsgType::Rpc_Consumer:

            break;
        case MsgType::Rpc_Method:

            break;
        };
    }

    void RpcRegistry::sendMes(Channel::ptr channel)
    {
        auto protocol = Protocol::createHealthPacket(1);

        protocol->printMes();

        auto mes = protocol->result();

        channel->send({mes.c_str(), mes.size()});
    }

}
