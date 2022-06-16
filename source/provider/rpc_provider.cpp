#include "provider/rpc_provider.h"

#include <mutex>
#include <optional>

#include "base/log/logger.h"
#include "rpc/heart_manager.h"
#include "net/channel.h"

#include <iostream>

namespace Fish
{
   RpcProvider::RpcProvider(TcpAddr addr, std::string name)
       : TcpServer(addr, name)
   {
   }

   void RpcProvider::begin()
   {
      TcpServer::setReadCallBack([this](Channel::ptr channel)
                                 { handleRead(channel); });

      TcpServer::setCloseCallBack([this](int fd)
                                  { handleClose(fd); });

      TcpServer::setNewCallBack([this](int fd)
                                { handleNew(fd); });

      TcpServer::begin();
   }

   void RpcProvider::bindRegistry(TcpAddr addr)
   {
      if (providerChannel_)
      {
         LOG_FATAL << "重复绑定端口" << end;
         return;
      }

      providerChannel_ = TcpServer::createConnection(addr);

      if (!providerChannel_)
      {
         std::cout << "无法连接至注册中心：" << addr.toStr() << std::endl;
         return;
      }
         
      auto protocol = ProtocolManager::create(MsgType::FRPC_PROVIDER, 0, "");

      providerChannel_->send(protocol->result());
   }

   void RpcProvider::handleRead(Channel::ptr channel)
   {
      auto protocol = protocols_.readMes(channel);

      if (!protocol)
         return;

      assert(protocol->getState() == RPCSTATE::READY);

      auto type = protocol->getType();

      switch (type)
      {
      case MsgType::FRPC_NONSENCE:
         LOG_FATAL << "不应该出现的情况..." << end;
         break;

      case MsgType::FRPC_HBEAT:
         hanleHealth(protocol, channel);

         break;
         //????怎么会收到这种type的消息
      case MsgType::FRPC_PROVIDER:

         break;
      };
   }

   void RpcProvider::hanleHealth(Protocol::ptr &protocol, Channel::ptr channel)
   {
      auto targetId = protocol->getId();

      auto [selfId, heartId] = HeartManager::contentDecode(protocol->getContent());

      if (selfId == -1 or heartId == -1)
      {
         LOG_DEBUG << "无法解析的心跳包数据" << end;
         return;
      }

      if (id_ != selfId)
      {
         LOG_DEBUG << "收到非本机的心跳包 id = " << selfId << end;
         return;
      }
      auto content = HeartManager::contentEncode(targetId, heartId);

      auto sendProtocol = ProtocolManager::createHeart(id_, content);

      channel->send(sendProtocol->result());
   }

   void RpcProvider::handleClose(int fd)
   {
      // cout << "rpc provider close: " << fd << endl;
   }
   void RpcProvider::handleNew(int fd)
   {
      // cout << "rpc provider new: " << fd << endl;
   }
}