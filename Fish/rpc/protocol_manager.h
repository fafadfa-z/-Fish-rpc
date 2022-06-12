#pragma once

#include "protocol.h"

#include <shared_mutex>

#include <map>

namespace Fish
{
    class Channel;

    class ProtocolManager
    {
    public:
        Protocol::ptr readMes(std::shared_ptr<Channel> &); //从channel 中读取数据

        [[nodiscard]]static Protocol::ptr create(MsgType, uint16_t, const std::string&); //通用的create函数

        [[nodiscard]]static Protocol::ptr createHeart(uint16_t, const std::string&); //创建心跳包

        [[nodiscard]]static Protocol::ptr createProviderResponce(uint16_t, const std::string&); //创建申请成为provider的回应包

        [[nodiscard]]static Protocol::ptr createMothodPull(uint16_t); //创建从provider服务器上拉取方法请求包。

    private:
        //储存只接收了部分消息的数据
        std::map<int, Protocol::ptr> undoProtols_;
        std::shared_mutex protocalMut_;
    };

}