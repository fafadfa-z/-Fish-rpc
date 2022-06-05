#include "provider/rpc_provider.h"

#include <mutex>
#include <optional>

#include "base/log/logger.h"
#include "net/channel.h"

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


   void RpcProvider::handleRead(Channel::ptr channel)
   {
      auto protocol = protocols_.readMes(channel);

      if(!protocol) return;

      assert(protocol->getState() == RPCSTATE::READY);

      auto type = protocol->getType();

      switch(type)
      {
         case MsgType::FRPC_NONSENCE:
            LOG_FATAL<<"不应该出现的情况..."<<Fish::end;
            break;

         case MsgType::FRPC_HBEAT:
            hanleHealth(protocol, channel);

         break;
         case MsgType::FRPC_PROVIDER:


         break;

      };
   }


   void RpcProvider::hanleHealth(Protocol::ptr& protocol, Channel::ptr channel)
   {
      assert(protocol->getType() == MsgType::FRPC_HBEAT);

      LOG_INFO<<TcpServer::name()<<" Handle health detection"<<Fish::end;

      std::optional<Protocol::ptr> responce;

      switch(protocol->getId())
      {
         case 1: //心跳包发送
            // responce = Protocol::createHealthPacket(id_, 2);

            refreshHealth(channel->fd());  //更新心跳信息(需要之后完成)

         break;
         case 2: //心跳包回应（理论上收不到这个包）


         default:
            LOG_DEBUG<<TcpServer::name()<<" unexpected health type..."<<Fish::end;
         break;
      };

      auto result = protocol->result();

      if(responce) channel->send(result);
   }



   void RpcProvider::handleClose(int fd)
   {
      // cout << "rpc provider close: " << fd << endl;

   }
   void RpcProvider::handleNew(int fd)
   {
      // cout << "rpc provider new: " << fd << endl;

   }

   void RpcProvider::refreshHealth(int fd)
   {




   }

}