#pragma

#include<net/tcp_server.h>



namespace Fish
{
    /*
    * @brief rpc消费者类
        具有的功能：
            1、注册
            2、调用函数
            3、发起调用（包含异步和同步调用）
            4、等待回应
            5、
    */
    class RpcCustomer : public TcpServer
    {

        public:
        //addr是server的addr
        RpcCustomer(TcpAddr addr, std::string_view cName = "customer");
    
        //发起调用
        template<typename... Parmas>
        bool regsFunc(const std::string_view func,Parmas... parm)
        {

        }
        
        //发起异步调用
        template<typename... Parmas>
        void regsFunc(){}


        //回应心跳包
        void responseHeartBeat();
        private:
            

        private:
            TcpAddr addr_;
            std::string_view cName_;

    };
} // namespace Fish
