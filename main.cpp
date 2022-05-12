#include <iostream>
#include "rpc/rpcProvider.h"

#include "rpc/protocol.h"


using namespace std;

vector<int> fun(vector<int>vec)
{
    vector<int>result;
    
    cout<<"传入参数: "<<endl;

    for(auto num : vec)
    {
        cout<<num<<"\t";
        result.push_back(num+3);
    }
    cout<<endl;

    return result;
}

int main()
{
    std::function<vector<int>(vector<int>)> f(fun);

    Fish::rpc::RpcProvider s({"127.0.0.1", 8848});

    

    s.registerMethod("fun",f);

    std::vector<int> vec{1,2,3,4,5};

    Fish::rpc::Serializer ss;

    ss<<vec;

    Fish::rpc::Serializer ret;

    s.run("fun",ss,ret);

    std::vector<int> retVec;

    ret>>retVec;

    cout<<"now output answer:\n";
    for(auto num : retVec)
        cout<<num<<"\t";

    cout<<endl;

    return 0;
}