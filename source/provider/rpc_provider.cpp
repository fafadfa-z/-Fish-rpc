#include "rpc/rpc_provider.h"
#include "net/channel.h"

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

      cout << "rpc provider read: " << channel->fd() << endl;
      
      auto view = channel->disView();

      cout <<view<<endl;

      bool oldFlag = false;

      Protocol::ptr protocol;

      {
         std::shared_lock<std::shared_mutex> guard(protocalMut_);

         auto itor = undoProtols_.find(channel->fd());

         if (itor == undoProtols_.end())
            protocol = std::make_shared<Protocol>();
         else
         {
            protocol = itor->second;
            oldFlag = true;
         }
      }

      auto [flag, size] = protocol->create(view);

      channel->eraseFront(size);

      if (flag and oldFlag) //如果上一次处理的数据完成了。
      {
         std::unique_lock<std::shared_mutex> guard(protocalMut_);

         undoProtols_.erase(undoProtols_.find(channel->fd()));
      }
      else if ((!flag) and (!oldFlag)) //如果来了新的，未完成的连接
      {
         std::unique_lock<std::shared_mutex> guard(protocalMut_);

         undoProtols_.emplace(channel->fd(), protocol);
      }

      if(flag)
         handleMessage(protocol);
  
   }

   void RpcProvider::handleMessage(Protocol::ptr& protocol)
   {
      assert(protocol->process() == ProtocolProcess::perfect);

      auto type = protocol->type();

      switch(type)
      {
         case MsgType::Rpc_None:
            LOG_FATAL<<"不应该出现的情况..."<<Fish::end;

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