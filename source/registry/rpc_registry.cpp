#include "registry/rpc_registry.h"

#include "net/channel.h"
#include "base/log/logger.h"

#include "base/timer.h"

namespace Fish
{
    RpcRegistry::RpcRegistry(TcpAddr addr, std::string name)
        : TcpServer(addr, name)
    {
    }

    void RpcRegistry::begin()
    {
        TcpServer::setTimer(Timer::init(100));

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
            LOG_DEBUG << "不应该出现的情况..." << Fish::end;
            break;

        case MsgType::Rpc_Health:
            // hanleHealth(protocol, channel);

            break;
        case MsgType::Rpc_Provider:
            handleProvider(protocol, channel);

            break;
        case MsgType::Rpc_Consumer:

            break;
        case MsgType::Rpc_Method:

            break;
        };
    }

    void RpcRegistry::handleProvider(Protocol::ptr &protocol, Channel::ptr &channel)
    {
        assert(protocol->type() == MsgType::Rpc_Provider);

        switch (protocol->id())
        {
        case 1: //注册成为provider

            providerNew(protocol, channel);

            break;
        };
    }

    void RpcRegistry::providerNew(Protocol::ptr &protocol, Channel::ptr &channel)
    {

        auto now = Timer::getNow_Milli();
        ProviderMes mes;

        mes.addr = channel->addr();
        mes.status = ProviderStatus::health;
        mes.lastHeart_send = now;
        mes.lastHeart_send = now;
        mes.channel = channel;
        
        {
            LockGuard<Mutex> guard(providers_mut_);
            mes.id = providers_.size();
            providers_.push_back(std::move(mes));
        }

        // TcpServer::timer_->addPriodTask();
    }

    void RpcRegistry::healthDetection(uint16_t id)
    {


    }

    // void RpcRegistry::sendMes(Channel::ptr channel)
    // {
    //     auto protocol = Protocol::createHealthPacket(1);

    //     protocol->printMes();

    //     auto mes = protocol->result();

    //     channel->send({mes.c_str(), mes.size()});
    // }

}
