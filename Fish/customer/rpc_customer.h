#pragma

#include<future>

#include<net/tcp_server.h>
#include<rpc/serializer.h>
#include"rpc/protocol_manager.h"


namespace Fish
{
    class Channel;
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
        //addr是customer的addr
        RpcCustomer(TcpAddr addr, const std::string cName = "customer");

        public:
    
        //发起调用
        template<typename... Parmas>
        void regsFunc(const std::string_view func,Parmas... parm)
        {
            Serializer args;
            args << func << parm;
            sendRequest(args);            
        }

        //无参数的调用
        template<typename... Parmas>
        void regsFunc(const std::string_view func)
        {
            Serializer args;
            args << func ;
            sendRequest(args);  
        }
        
        //发起异步调用
        /**
         *  @brief 返回一个future，然后通过std::future<R> f = callFunc()，调用结果可以通过f->get得到
        **/
        template<typename Result,typename... Parmas>
        std::future<Result> callFunc(const std::string_view func, Parmas... parm)
        {       

            std::function<Result> task = [&func,&parm...,this](){return regsFunc(func,parm);};

            std::promise<Result> p = std::make_shared<std::promise<Result>> ();
            p->set_value(task());
            return p->get_future();      
        }
        /**
         *  @brief 重载函数，当调用无需参数的函数时，
         *          返回一个future，然后通过std::future<R> f = callFunc()，调用结果可以通过f->get得到
        **/
        template<typename Result,typename... Parmas>
        std::future<Result> callFunc(const std::string_view func)
        {       

            std::function<Result> task = [&func,this](){return regsFunc(func);};

            std::promise<Result> p = std::make_shared<std::promise<Result>> ();
            p->set_value(task());
            return p->get_future();      
        }


        /**
         *  @brief 初始化后，通过begin()函数，启动连接。
         * 
        **/
        void begin() override;

        
        /**
         * @brief /重载绑定注册中心函数，默认使用无参数的
         * 
         */
        void bindRegistry();
        void bindRegistry(TcpAddr);
        
        private:
            /**
             * @brief 将序列化好的消息，发送出去，所有的调用函数，最终都是通过sendRequest发送
             * 
             * @param args 
             * @return Protocol::ptr 
             */
            std::string sendRequest(Serializer& args);

            
            /**
             * @brief 处理收到的响应数据
             * 
             * @param cl_ 
             */
            void handleRead( std::shared_ptr<Channel> cl_);

            /**
             * @brief 
             * 
             * @param fd 
             */
            void handleNew(int fd);
            void handleClose(int fd);            
            
            /**
             * @brief 回应心跳包
            **/
            void responseHeartBeat(Fish::Protocol::ptr& , std::shared_ptr<Channel>);

            //在send中创建协议文件，并发送

            /**
             * @brief 设置请求返回结果的回调函数
             * 
             */
            void setRR(std::function<void(Fish::Protocol::ptr&)> rrcb) {requeryResponse_ = rrcb;};
            
        private:

            TcpAddr addr_;
            std::string cName_;
            uint8_t myId_;
        private:

            std::shared_ptr<Channel> customerChannel_;

            ProtocolManager protocolManager_;
          
            //创建一个回调函数指针，表示发送的请求返回的结果
            std::function<void(Fish::Protocol::ptr&)> requeryResponse_; 


            //通过promise与future，形成异步调用，在send函数中获得读取的结果
            std::shared_ptr<std::promise<std::string>> RePromise;
            std::future<std::string> ReFuture;

    };
} // namespace Fish
