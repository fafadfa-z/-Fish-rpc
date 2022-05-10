#include <iostream>
#include "rpc/rpcProvider.h"

#include "rpc/protocol.h"

using namespace std;

int fun(int a, char b)
{

    return a + (int)b;
}

int main()
{
    std::function<int(int, int)> f(fun);

    Fish::rpc::RpcProvider s({"127.0.0.1", 8848});

    // s.registerMethod("fun",f);

    std::vector<uint8_t> vec;

    std::pair<int, int> pa;

    // Fish::rpc::FunPacket p;

    Fish::rpc::findType(pa, vec);

    for (auto num : vec)
    {
        cout << (int)num << endl;
    }

    // cout<<"Hello!"<<endl;

    // Fish::TcpAddr addr("39.108.131.90",8848);

    // Fish::TcpServer server(addr);

    // server.begin();

    return 0;
}
