#pragma once

#include <limits.h>
#include <optional>
#include <string>
#include <list>

namespace Fish
{
    class HeartManager
    {
    private:
        struct heatTask
        {
            int64_t sendTime = 0;
            int64_t recvTime = 0;
        };

    public:
        HeartManager(int maxSize = 3) :maxSize_(maxSize){}

        // 发送心跳检测，并生成协议包的内容部分
        // 如果返回的数据中没有内容，则该链接被判断为不可达
        std::optional<std::string> sendHeart();

        auto setId(auto id) {id_ = id;}

        void clearMessage(); //清空所有统计信息, 心跳包从 heart_id_ = 1 开始发

    private:

        bool alive(); //判断是否心跳检测失败

        void updateMessage(heatTask&);  // 更新统计信息

        void updateList(heatTask&&);    // 更新节点信息 

    private:
        int maxSize_; //允许的最大连续心跳包失败数

        std::list<heatTask> list_;

        uint16_t id_ = 0;           // 由外界指定的id
        uint32_t heart_id_ = 1;     // 随心跳包递增的id

        float lossRate_=1.0f;         // 丢包率
        uint32_t aveTime=0;         // 平均响应时间

    };

}