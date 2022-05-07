#include <iostream>

#include "net/tcp_server.h"

using namespace std;








int main()
{
    cout<<"Hello!"<<endl;
    
    Fish::TcpAddr addr("39.108.131.90",8848);

    Fish::TcpServer server(addr);

    server.begin();


    return 0;
}

