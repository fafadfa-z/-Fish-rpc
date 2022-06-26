#include<customer/rpc_customer.h>
#include"rpc/protocol_manager.h"
#include"rpc/heart_manager.h"

#include"net/channel.h"
#include"base/log/logger.h"

 namespace Fish
 {
    RpcCustomer::RpcCustomer(TcpAddr addr,const std::string cName = "customer")
                :TcpServer(addr,cName)     
                ,addr_(addr)
                ,cName_(cName)       
    {
        
        
    }

    void RpcCustomer::begin()
    {
        TcpServer::setNewCallBack([this](int fd_)  {return handleNew(fd_) ;});
        TcpServer::setCloseCallBack([this](int fd_){ return handleClose(fd_);});
        TcpServer::setReadCallBack([this](Channel::ptr channel_){return handleRead(channel_);});

        TcpServer::begin();
        bindRegistry();

        ReFuture = RePromise->get_future();
    }

    void RpcCustomer::bindRegistry(TcpAddr addr)
    {
        if(customerChannel_)
        {
            LOG_FATAL << "重复绑定端口" << end;
            return;
        }
        customerChannel_ = TcpServer::createConnection(addr);
        if(!customerChannel_)
        {
            std::cout << "无法连接至注册中心：" << addr.toStr() << std::endl;
            return;
        }

        auto cProtocol = ProtocolManager::create(MsgType::FRPC_CONSUMER,0,"");
        customerChannel_->send(cProtocol->result());
    }

    void RpcCustomer::bindRegistry()
    {
         if(customerChannel_)
        {
            LOG_FATAL << "重复绑定端口" << end;
            return;
        }
        customerChannel_ = TcpServer::createConnection(addr_);
        if(!customerChannel_)
        {
            std::cout << "无法连接至注册中心：" << addr_.toStr() << std::endl;
            return;
        }

        auto cProtocol = ProtocolManager::create(MsgType::FRPC_CONSUMER,0,"");
        customerChannel_->send(cProtocol->result());
    }

   
    void RpcCustomer::responseHeartBeat(Fish::Protocol::ptr& protocol,Channel::ptr channel)
    {
        auto targetId = protocol->getId();

        auto [selfId,heartId] = HeartManager::contentDecode(protocol->getContent());

        if(selfId == -1 || heartId == -1)
        {
            LOG_DEBUG << "无法解析的心跳包数据,Id(-1) 错误" <<  end;
            return;
        }

        if(myId_ != selfId) 
        {
            LOG_DEBUG << "收到非本机的心跳包 id = " << selfId << end;
            return;
        }
        auto content = HeartManager::contentEncode(targetId, heartId);

        auto sendProtocol = ProtocolManager::createHeart(myId_, content);

        channel->send(sendProtocol->result());
    }

    std::string RpcCustomer::sendRequest(Serializer &args)
    {
        std::string t;
        args >> t;

        auto cProtocol = ProtocolManager::create(MsgType::FRPC_CREQUEST,myId_,t);

        customerChannel_->send(cProtocol->result());

        auto s = ReFuture.get();
        return s;
    }

    void RpcCustomer::handleRead(Channel::ptr channel)
    {
        auto protocolTmp = protocolManager_.readMes(channel);
        if(!protocolTmp)    return ;

        assert(protocolTmp->getState() == RPCSTATE::READY);

        switch (protocolTmp->getType())
        {
        case MsgType::FRPC_NONSENCE :
            LOG_FATAL << "不应该出现的情况..." << end;
            break;
        case MsgType::FRPC_HBEAT:
            responseHeartBeat(protocolTmp,channel);
            break;
        case MsgType::RPC_METHOD_CRESPONSE:
            auto task = [&protocolTmp]()->std::string {return protocolTmp->getContent();};
            RePromise->set_value(task());            
            break;
        }

    }  

    void RpcCustomer::handleNew(int fd){
        
    }
    void RpcCustomer::handleClose(int fd){}

    
 } // namespace Fish
 
