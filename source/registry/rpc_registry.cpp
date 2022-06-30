#include "registry/rpc_registry.h"

#include "net/channel.h"
#include "base/log/logger.h"

#include "base/timer.h"

namespace Fish
{
    RpcRegistry::RpcRegistry(TcpAddr addr, std::string name, uint16_t id)
        : TcpServer(addr, name), id_(id)
    {
        nodeManager_.setEraseCallBack([this](uint16_t id)
                                { providerErase(id); });
    }

    void RpcRegistry::begin()
    {
        TcpServer::setTimer(Timer::init(100));

          

        TcpServer::setReadCallBack([this](Channel::ptr channel)
                                   { handleMessage(channel); });


        TcpServer::setCloseCallBack([this](int fd)
                                  { handleClose(fd); });


        TcpServer::begin();

        // TcpServer::createConnection({"127.0.0.1", 8848});

        auto timer = Timer::init(100);

        timer->start();
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
            LOG_DEBUG << "不应该出现的情况: FRPC_NONSENCE " << Fish::end;
            break;

                                

        case MsgType::FRPC_PROVIDER:
            providerNew(protocol, channel);
            break;

        case MsgType::FRPC_CONSUMER:

            break;

        default:
            LOG_DEBUG << "不应该出现的情况:" << static_cast<int>(type) << Fish::end;
            break;
        };
    }

#include <iostream>
    using namespace std;

    void RpcRegistry::providerNew(Protocol::ptr &protocol, Channel::ptr &channel)
    {
        // auto id = nodes_.createId(); // 为新来的provider生成一个id号

        // cout << "新provider到来, 为其分配id: " << id << endl;

        // NodeManager::NodePtr newProvider = std::make_shared<ProviderNode>(channel,id_, id);

        // nodes_.addNode(newProvider);

        //添加心跳检测定时器
        // bool ret = TcpServer::timer_->addPriodTask([id, this]
        //                                            { nodes_.addInTimer_heart(id); },
        //                                            5000, id);

        // assert(ret == true);

        // auto packAck = ProtocolManager::createProviderResponce(id_, std::to_string(id)); //返回生成的id

        // auto packPull = ProtocolManager::createMothodPull(id_); //创建向provider拉取方法的请求

        // channel->send(packAck->result() + packPull->result());

        // channel->send(packAck->result());
    }

    void RpcRegistry::providerErase(size_t id) //此函数为NodeManager的回调
    {
        bool ret = TcpServer::timer_->removePriodTask(id);

        assert(ret == true);
    }


    void RpcRegistry::handleClose(int fd)
    {

    }
}
