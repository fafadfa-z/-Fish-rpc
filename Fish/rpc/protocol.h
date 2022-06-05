#pragma once


#include<memory>
#include<string>
#include<string_view>
#include"string.h"
#include<deque>
#include<limits.h>


/*
    尝试设计一个新的私有通信协议SUN
    +-------+--------------+-------------+----------+------+-----------------+-------+------------------------+-------------+-------------+
    |Magic  | total length | head length |Version   |type  | serialize format|  id   | extend                 |contentLength|   content   |
    +-------+--------------+-------------+----------+------+-----------------+-------+------------------------+-------------+-------------+
    |1      |       1      |      1      |   1      | 1    |        1        |   4   |totalLength - headLength|    4        |  xx         |
    ---------------------------------------------------------------------------------------------------------------------------------------

    第一个字节是魔术位，
    第二个字节是总长度
    第三个字节是正常头部长度，
    第四个字节是版本号，
    第五个字节是传入消息的类型
    第六个字节是序列化方式，
    第七个字节是序列号id，使用4个字节表示。模仿TCP，序列号增加。
    第十一个是extend。
    第十二个字节是文本长度。
    第十六个字节开始是传输文本。
*/  


namespace Fish
{
    enum MsgType
    {
        FRPC_NONSENCE = (uint8_t)0,  //无意义

        FRPC_HBEAT,     //心跳包
        FRPC_PROVIDER,  //申请成为服务提供商
        FRPC_CONSUMER,  //申请成为服务消费者

        FRPC_PREQUEST,  //服务提供商的请求
        FRPC_CREQUEST,  //服务消费者的请求
        FRPC_PRESPONSE, //服务提供商的响应
        FRPC_CRESPONSE, //服务消费者的响应

        RPC_METHOD_PREQUEST,    //   服务提供商请求方法调用
        RPC_METHOD_PRESPONSE,
        RPC_METHOD_CREQUEST,
        RPC_METHOD_CRESPONSE
    };
    enum RPCSTATE
    {
        WORKING,        //已经有正在处理的信息流
        FREE,           //处于空闲状态
        READY   ,       //写入的信息，但是还没有处理
        WAITCONTENT,       //等待写入content
        LACK,           //写入信息不完整
    };

    class Protocol
    {
        public:
            Protocol();
            // ~Protocol();
        
        public:
            using ptr = std::shared_ptr<Protocol>;
        
        public:    
            //创建一个新的协议流程
            int create(std::string_view);
            //创建符合协议的消息流
            //static Protocol::ptr msgCreate(std::string_view);
            //创建心跳包
            static Protocol::ptr hbCreate(uint32_t id = 0);
            //创建注册Provider包
            static Protocol::ptr pvdCreat(uint32_t id = 0);
            //创建注册Consumer包
            static Protocol::ptr csmCreate(uint32_t id = 0);
        public:
            //获取参数
            uint8_t  getMagic() const { return msg_._msg.magic_;}
            uint32_t getLength() const { return msg_._msg.headLength_;}
            uint32_t getAppendLength() const { return msg_._msg.totalLength_ - msg_._msg.headLength_;}
            uint8_t  getVersion() const {return msg_._msg.version_;}
            uint16_t getType() const { return msg_._msg.msgType_;}
            uint8_t  getSFormat() const { return msg_._msg.sFormat_;}
            uint32_t getId() const { return msg_._msg.msgId_;}
            uint32_t getState() const { return handleState;}
            std::string getContent() const { return content_;}

            //设置参数
            void setMagic(uint8_t magic) {msg_._msg.magic_ = magic;}
            void setAppendLength(uint32_t adLength) { msg_._msg.totalLength_ = adLength + msg_._msg.headLength_;};
            void setVersion(uint8_t version) { msg_._msg.version_ = version;}
            void setMsgType(uint16_t msgType) { msg_._msg.msgType_ = msgType;}
            void setSFormat(uint8_t format) { msg_._msg.sFormat_ = format;}
            void setMsgId(uint32_t id) { msg_._msg.msgId_ = id;}      
            void setContent(const std::string& content);
            void setLength(uint32_t length) ;
            //void editAppend(std::string_view&);



            //输出所有信息，用于调试
            void printInfo();
            std::string result();
                
        protected:
            std::string gettime();
        private:
            static constexpr uint8_t dfMagic = 0xcc;
            static constexpr uint16_t dfVersion = 1;
            static constexpr uint8_t dfHeadLength = 15;
            static constexpr uint8_t apdLength = 0;
        private:             
            union headMsg
            {
            
                // ~headMsg();
                struct alignas(4) Msg
                {
                    uint8_t  magic_ ;
                    uint8_t  totalLength_ ;
                    uint8_t  headLength_ ; 
                    uint8_t  version_ ;
                    uint16_t msgType_;
                    uint8_t  sFormat_;
                    uint32_t msgId_;  
                    //std::string  extend_ = "";
                    uint32_t contentLength_;
                };
                Msg _msg ;
                char msgStr[dfHeadLength + 1 + apdLength];
            };

            union headMsg msg_;
            void setHeadMsg(MsgType type,uint32_t id);
            
        private:
            std::string content_;
            RPCSTATE handleState = RPCSTATE::FREE;
        
    };
}