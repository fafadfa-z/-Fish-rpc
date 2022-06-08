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
            uint16_t id = 0;         //心跳包的序列号
            bool statistics = false;
        };

        union HeartData
        {
            struct Data
            {
                uint16_t heartId_ = 0;   //心跳包的id
                uint16_t targetId_ = 0;  //目标id
            };
            Data data;

            char buf[sizeof(data)] = {0};
        };

    public:
        HeartManager(uint16_t self, uint16_t target, size_t maxSize = 3);

        ~HeartManager() {}

        // 发送心跳检测，并生成协议包的内容部分
        // 如果返回的数据中没有内容，则该链接被判断为不可达
        std::optional<std::string> sendHeart();

        //识别心跳检测传回来的数据
        void recvHeart(const std::string &);


        const auto getSelfId() const {return selfId_;} 
        const auto getTargetId() const {return targetId_;}


        float lossRatio() //返回丢包率
        {
            return static_cast<float>(lossNum_) / totalNum_;
        }

        //返回平均响应时间(单位：毫秒)
        int16_t aveTime()
        {
            return lossNum_ / (totalNum_ - lossNum_);
        }

        void clearMessage(); //清空所有统计信息, 心跳包从 heart_id_ = 1 开始发

    private:
        bool alive(); //判断是否心跳检测失败

        void updateMessage(); // 更新统计信息

        void insertNode(heatTask &&); // 更新节点信息

    private:
        size_t maxSize_; //允许的最大连续心跳包失败数

        std::list<heatTask> list_;

        uint16_t selfId_ = 0;   // 由外界指定的自身id
        uint16_t targetId_ = 0; // 由外界指定的对方id

        int64_t beginTime_ = 0;  // 检测开始的时间
        uint64_t totalTime_ = 0; // 总响应时间
        uint32_t totalNum_ = 0;  // 总心跳次数
        uint16_t lossNum_ = 0;   // 丢包次数
    };

}