#include "provider/rpc_provider.h"


#include <mutex>

#include "base/log/logger.h"

namespace Fish
{
   RpcProvider::RpcProvider(TcpAddr addr, std::string name)
       : TcpServer(addr, name)
   {
   }

   void RpcProvider::begin()
   {
      TcpServer::setReadCallBack([this](std::shared_ptr<Channel> channel)
                                 { handleRead(channel); });

      TcpServer::setCloseCallBack([this](int fd)
                                  { handleClose(fd); });

      TcpServer::setNewCallBack([this](int fd)
                                { handleNew(fd); });

      TcpServer::begin();
   }


   void RpcProvider::handleRead(std::shared_ptr<Channel> channel)
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

         break;
         case MsgType::Rpc_Provider:


         break;
         case MsgType::Rpc_Consumer:


         break; 
         case MsgType::Rpc_Method:

         break;
      };


   }



   void RpcProvider::handleClose(int fd)
   {
      cout << "rpc provider close: " << fd << endl;

      // channel->send(view);
   }
   void RpcProvider::handleNew(int fd)
   {
      cout << "rpc provider new: " << fd << endl;

      // channel->send(view);
   }

}