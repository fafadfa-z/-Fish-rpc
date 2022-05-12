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

    enum ProtocolProcess
    {
        empty,        //空的
        need_content, //收到了完整的包头信息，还剩下具体内容
        perfect       //收到了完整的数据
    };

    class Protocol // 通信协议的封装
    {
    public:
        using ptr = std::shared_ptr<Protocol>;

        Protocol() = default;

        auto magic() const { return mes_.mes.magic_; }
        auto version() const { return mes_.mes.version_; }
        auto type() const { return mes_.mes.type_; }
        auto id() const { return mes_.mes.id_; }
        auto size() const { return mes_.mes.size_; }

        const auto &content() const { return content_; }

        void setMagic(uint8_t magic) { mes_.mes.magic_ = magic; }
        void setVersion(uint8_t version) { mes_.mes.version_ = version; }
        void setType(uint16_t type) { mes_.mes.type_ = type; }
        void setId(uint16_t id) { mes_.mes.id_ = id; }
        void setSize(uint64_t size) { mes_.mes.size_ = size; }

        bool setContent(const std::string &); //设置内容
        bool setContent(const char *);
        bool setContent(const char *, size_t);
        bool setContent(const std::string_view);
        bool setContent(const std::deque<char> &);

        void calcSize(); //重新计算总长度


        std::string result();

        /**
         * @brief 将输入的数据流转化成数据包
         * @param[in] view 输入的数据流
         * @param[out] first 是否得到了一个完整的包
         * @param[out] second 从输入中读取了数据的长度，这些数据需要删除
         */
        std::pair<bool, size_t> create(std::string_view view); //根据接收到的数据流生成协议包

        static ptr createHealthPacket(uint16_t = 0); //创建心跳包

        static ptr createProvider(uint16_t = 0); //创建注册成为Provider的包

        static ptr createConsumer(uint16_t = 0); //创建注册成为Consumer的包

    private:
        std::string content_;

        ProtocolProcess process_ = ProtocolProcess::empty;

    private:
        inline static constexpr uint8_t default_magic_ = 0xef;
        inline static constexpr uint8_t default_version_ = 0x01;
        inline static constexpr uint8_t Base_length = 10;
        inline static constexpr uint64_t Max_size = UINT32_MAX;

        
        union headMes
        {
            struct alignas(4) Mes
            {
                uint8_t magic_ ;
                uint8_t version_;
                uint16_t type_;
                uint32_t size_;
                uint16_t id_;
            };
            Mes mes;
            char mesStr[Base_length + 2]; //加2 是因为内存对齐的原因，
        };

        headMes setHeadMes(MsgType,uint16_t);

        union headMes mes_;

        static_assert(sizeof(headMes::Mes) == sizeof(headMes::mesStr), "size error");
    };
}
