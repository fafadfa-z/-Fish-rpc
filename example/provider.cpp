#include <iostream>
#include "rpc/rpcProvider.h"

using namespace std;






int main()
{
    cout<<"this is provider"<<endl;

    Fish::RpcProvider s({"127.0.0.1", 8848});

    s.begin();


    return 0;
}






