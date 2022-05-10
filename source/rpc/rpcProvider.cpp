#include "rpc/rpcProvider.h"

namespace Fish::rpc
{
   RpcProvider::RpcProvider(TcpAddr addr,std::string name)
    :TcpServer(addr,name)
   {




   }
}