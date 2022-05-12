#include "rpc/rpcProvider.h"
#include "net/channel.h"

namespace Fish
{
   RpcProvider::RpcProvider(TcpAddr addr,std::string name)
    :TcpServer(addr,name)
   {




   }

   void RpcProvider::begin()
   {
      TcpServer::begin();

      setReadCallBack([this](Channel::ptr channel){handleMessage(channel);});

   }




   void RpcProvider::handleMessage(Channel::ptr channel)
   {





   }
}