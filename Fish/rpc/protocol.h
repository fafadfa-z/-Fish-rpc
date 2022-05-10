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

namespace Fish::rpc
{


    enum MsgType
    {
        Rpc_None, //无意义的包

        Rpc_Health, // 心跳包
                    // 0: 心跳包发送
                    // 1: 心跳包回应

        Rpc_Provider, // 向服务中心注册为 provider
                      // 1: 注册成为provider
                      // 2: 注册成功
                      // 3: 注册失败
                      // 4: provider下线
                      // 5: provider下线的回应
                      // 6: 注册支持的方法
                      // 7: 注册支持的方法成功
                      // 8: 注册支持的方法失败
                      // 9: 删除支持的方法
                      // 10: 删除支持的方法成功
                      // 11: 删除支持的方法失败

        Rpc_Consumer, // 1: 向服务中心注册为 consumer
                      // 2: 注册成功
                      // 3: 注册失败
                      // 4: consumer下线
                      // 5: consumer下线的回应

        Rpc_Method, // 请求方法调用
                    // 1: 函数的具体格式(带函数名称, id)
                    // 2: 函数的返回值数据(带函数名称, id)
                    // 3: 函数的参数类型(带函数名称, id)
                    // 4: 调用失败，返回错误信息 (带函数名称, id)
                    // 5: 调用成功，返回结果的类型(带函数名称, id)
                    // 5: 调用成功，返回结果的数据(带函数名称, id)、
    };

    class Protocol // 通信协议的封装
    {
    public:
        using ptr = std::shared_ptr<Protocol>;

        Protocol() = default;

        auto magic() const { return magic_; }
        auto version() const { return version_; }
        auto type() const { return type_; }
        auto id() const { return id_; }
        auto size() const { return size_; }

        const auto &content() const { return content_; }

        void setMagic(uint8_t magic) { magic_ = magic; }
        void setVersion(uint8_t version) { version_ = version; }
        void setType(uint16_t type) { type_ = type; }
        void setId(uint16_t id) { id_ = id; }
        void setSize(uint64_t size) { size_ = size; }

        bool setContent(const std::string &); //设置内容
        bool setContent(const char *);
        bool setContent(const char *, size_t);
        bool setContent(const std::string_view);
        bool setContent(const std::deque<char> &);

        void calcSize(); //重新计算总长度

        static ptr create(MsgType, uint16_t = 0);

        static ptr createHealthPacket(uint16_t = 0); //创建心跳包

        static ptr createProvider(uint16_t = 0); //创建注册成为Provider的包

        static ptr createConsumer(uint16_t = 0); //创建注册成为Consumer的包

    private:
        uint8_t magic_ = default_magic_;
        uint8_t version_ = default_version_;
        uint16_t type_ = 0;
        uint16_t id_ = 0;
        uint64_t size_ = 0;

        std::string content_;

        bool wellFlag_ = false; //标志包是否可以发送了

    private:

        inline static constexpr uint8_t default_magic_ = 0xef;
        inline static constexpr uint8_t default_version_ = 0x01;
        inline static constexpr uint8_t Base_length = 10;
        inline static constexpr uint64_t Max_size = UINT32_MAX;
    };
}
