#include <iostream>
#include "registry/rpc_registry.h"

using namespace std;






int main()
{
    cout<<"this is provider"<<endl;

    Fish::RpcRegistry s({"127.0.0.1", 8888});

    s.begin();

    while(1) std::this_thread::sleep_for(2s);

    return 0;
}

