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

      assert(protocol->process() == ProtocolProcess::perfect);

      auto type = protocol->type();

      switch(type)
      {
         case MsgType::Rpc_None:
            LOG_FATAL<<"不应该出现的情况..."<<Fish::end;
            break;

         case MsgType::Rpc_Health:
            hanleHealth(protocol, channel);

         break;
         case MsgType::Rpc_Provider:


         break;
         case MsgType::Rpc_Consumer:


         break; 
         case MsgType::Rpc_Method:

         break;
      };
   }


   void RpcProvider::hanleHealth(Protocol::ptr& protocol, Channel::ptr channel)
   {
      assert(protocol->process() == ProtocolProcess::perfect);

      LOG_INFO<<TcpServer::name()<<" Handle health detection"<<Fish::end;

      std::optional<Protocol::ptr> responce;

      switch(protocol->id())
      {
         case 1: //心跳包发送
            responce = Protocol::createHealthPacket(id_, 2);

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