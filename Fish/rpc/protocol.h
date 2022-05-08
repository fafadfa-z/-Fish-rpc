#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <deque>
#include <limits.h>



#if 0 

    通信协议

    名称        magic    size  version   type   id

    大小(字节)    1       4       1        2     2

#endif

namespace Fish
{
    enum MsgType
    {
        Rpc_None,   //无意义的包
        
        Rpc_Health, //心跳包

        Rpc_Provider, // 向服务中心注册为 provider
        Rpc_Consumer, // 像服务中心注册为 consumer

        Rpc_Method_Request, // 请求方法调用
        Rpc_Method_Respone, // 相应方法调用

        Rpc_Serive_Register,         //像服务中心注册服务。
        Rpc_Serive_Register_Respone, //注册服务的相应报文

    };

    class Protocol  // 通信协议的封装
    {
    public:
        using ptr = std::shared_ptr<Protocol>;

        Protocol() = default;

        auto magic() const {return magic_;}
        auto version() const {return version_;}
        auto type() const {return type_;}
        auto id() const {return id_;}
        auto size() const {return size_;}

        const auto& content() const {return content_;}

        void setMagic(uint8_t magic)  {magic_ = magic;}
        void setVersion(uint8_t version)  {version_ = version;}
        void setType(uint16_t type)  {type_ = type;}
        void setId(uint16_t id)  {id_ = id;}
        void setSize(uint64_t size) {size_ = size;}
        

        void setContent(const std::string&);  //设置内容
        void setContent(const char*);
        void setContent(const char*, size_t);
        void setContent(const std::string_view);
        void setContent(const std::deque<char>&);

        void calcSize(); //重新计算总长度


        static ptr create(MsgType,uint16_t  = 0);

        static ptr createHealthPacket(uint16_t = 0);  //创建心跳包

        static ptr createProvider(const std::string&);  //创建注册成为Provider的包

        static ptr createConsumer(const std::string&);  //创建注册成为Consumer的包

    private:
        uint8_t magic_ = default_magic_;
        uint8_t version_ = default_version_;
        uint16_t type_ = 0;
        uint16_t id_ = 0;
        uint64_t size_ = 0; 

        std::string content_;

        bool wellFlag_ = false;  //标志包是否可以发送了

    private:
        inline static constexpr uint8_t default_magic_ = 0xef;
        inline static constexpr uint8_t default_version_= 0x01;
        inline static constexpr uint8_t Base_length = 10;
        inline static constexpr uint64_t Max_size = UINT32_MAX;
    };

}
