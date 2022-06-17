#include <iostream>
#include "registry/rpc_registry.h"

using namespace std;

int main()
{
    system("clear");
    
    cout << "this is registry" << endl;

    Fish::RpcRegistry s({"127.0.0.1", 8888},"rpc registry",666);

    s.begin();

    while (1)
        std::this_thread::sleep_for(2s);

    return 0;
}
