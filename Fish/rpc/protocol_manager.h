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

        static Protocol::ptr createHeart(const std::string&); //创建心跳包


    private:
        //储存只接收了部分消息的数据
        std::map<int, Protocol::ptr> undoProtols_;
        std::shared_mutex protocalMut_;
    };

}