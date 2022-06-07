#include "registry/rpc_registry.h"

#include "net/channel.h"
#include "base/log/logger.h"

#include "base/timer.h"

namespace Fish
{
    RpcRegistry::RpcRegistry(TcpAddr addr, std::string name, uint16_t id)
        : TcpServer(addr, name), id_(id)
    {
        nodes_.setEraseCallBack([this](uint16_t id){providerErase(id);});

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

        assert(protocol->getState() == RPCSTATE::READY);

        auto type = protocol->getType();

        switch (type)
        {
        case MsgType::FRPC_NONSENCE:
            LOG_DEBUG << "不应该出现的情况..." << Fish::end;
            break;

        case MsgType::FRPC_HBEAT:
            hanleHealth(protocol, channel);

            break;
        case MsgType::FRPC_PROVIDER:
            handleProvider(protocol, channel);

            break;
        case MsgType::FRPC_CONSUMER:

            break;
        };
    }

    void RpcRegistry::handleProvider(Protocol::ptr &protocol, Channel::ptr &channel)
    {
        assert(protocol->getState() == MsgType::FRPC_PROVIDER);

        switch (protocol->getId())
        {
        case 1: //注册成为provider

            providerNew(protocol, channel);
            break;
        case 2:

            break;


        };
    }

    void RpcRegistry::providerNew(Protocol::ptr &protocol, Channel::ptr &channel)
    {
        auto id = nodes_.createId(); // 为新来的provider生成一个id号

<<<<<<< HEAD
        auto now = Timer::getNow_Milli();
        ProviderMsg mes;

        mes.addr = channel->addr();
        mes.status = ProviderStatus::HEALTH;
        mes.lastHeart_send = now;
        mes.lastHeart_send = now;
        mes.channel = channel;
        
        {
            LockGuard<Mutex> guard(providers_mut_);
            mes.id = providers_.size();
            providers_.push_back(std::move(mes));
        }
=======
        NodeManager::NodePtr newProvider = std::make_shared<ProviderNode>(id_,id);

        nodes_.addNode(newProvider);
>>>>>>> origin/test

        //添加心跳检测定时器
        bool ret = TcpServer::timer_->addPriodTask([id,this]{nodes_.addInTimer_heart(id);},2000,id); 

        assert(ret == true);
    }

    void RpcRegistry::providerErase(size_t id)   //此函数为 NodeManager 的回调
    {
        bool ret = TcpServer::timer_->removePriodTask(id);

        assert(ret == true);
    }

    void RpcRegistry::healthWorker(size_t id) //心跳检测的处理函数
    {
        // auto& provider = providers_[id - 1];

        // auto content = provider.sendHeart();

            // if(content == std::nullopt)
        //     providerErase(id);   //心跳检测失败，断开连接

    }



    void RpcRegistry::hanleHealth(Protocol::ptr &protocol, std::shared_ptr<Channel> &channel)
    {
    }



}
