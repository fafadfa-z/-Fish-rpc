#include <iostream>
#include "provider/rpc_provider.h"

using namespace std;



int main()
{
    system("clear");
    
    cout<<"this is provider"<<endl;

    Fish::RpcProvider s({"127.0.0.1", 8848});

    s.begin();

    s.bindRegistry({"127.0.0.1", 8888});

    while(1)
    {
        std::this_thread::sleep_for(1s);
    }



    return 0;
}

