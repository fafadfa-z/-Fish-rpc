#include <iostream>
#include "rpc/serializer.h"
#include "net/tcp_server.h"
#include "rpc/protocol.h"

using namespace std;

int fun(int a, char b)
{

    return a + (int)b;
}


int main()
{
    Fish::rpc::Serializer s;

    int num = 5;
    s.input(num);




    // cout<<"Hello!"<<endl;
    
    // Fish::TcpAddr addr("39.108.131.90",8848);

    // Fish::TcpServer server(addr);

    // server.begin();


    return 0;
}

