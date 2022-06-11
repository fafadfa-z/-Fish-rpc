#include "rpc/heart_manager.h"
#include "base/timer.h"

#include <cassert>

#include <iostream>
using namespace std;

namespace Fish
{
    HeartManager::HeartManager(uint16_t self, uint16_t target, size_t maxSize)
        : maxSize_(maxSize), selfId_(self), targetId_(target)
    {
    }

    std::optional<std::string> HeartManager::sendHeart()
    {
        if (!alive())
            return std::nullopt;

        uint16_t id = 0;

        if (list_.empty())
            id++;
        else
            id = list_.back().id + 1;

        heatTask task{Timer::getNow_Milli(), 0, id};

        HeartData temp{0};

        temp.data.heartId_ = task.id;
        temp.data.targetId_ = targetId_;

        cout << "发送心跳包: heartid= " << task.id << "targetId= " << targetId_ << "selfId= " << selfId_ << endl;

        insertNode(std::move(task));

        return temp.buf;
    }

    void HeartManager::recvHeart(const std::string &content)
    {
        assert(content.size() == sizeof(HeartData));



        HeartData temp{0};

        std::copy(content.begin(), content.end(), temp.buf);

        cout << "收到心跳包: heartid= " << temp.data.heartId_ << " targetId= " << temp.data.targetId_ << endl;
        if (temp.data.targetId_ == selfId_) //验证id正确性
        {
            for (auto &node : list_)
            {
                if (node.id == temp.data.heartId_)
                {
                    node.recvTime = Timer::getNow_Milli();

                    updateMessage(); //更新统计信息
                    return;
                }
            }
        }
    }

    bool HeartManager::alive()
    {
        if(list_.size()<maxSize_) return true;

        for (auto &node : list_)
        {
            if (node.recvTime > node.sendTime) //只要有一个收到了，就没问题
                return true;
        }
        return false;
    }

    void HeartManager::insertNode(HeartManager::heatTask &&node)
    {
        [[unlikely]] if (list_.empty()) // 记录开始的时间
        {
            beginTime_ = Timer::getNow_Milli();
        }

        if (list_.size() < maxSize_)
        {
            list_.push_back(node);
        }
        else //将最前的节点插到后面
        {
            auto iter = list_.begin();
            list_.splice(list_.end(), list_, iter);

            iter->sendTime = node.sendTime;
            iter->recvTime = node.recvTime;
            iter->id = node.id;
        }
    }

    void HeartManager::updateMessage() // 更新统计信息
    {
        for (auto &node : list_)
        {
            if (!node.statistics)
            {
                node.statistics = true;
                totalNum_++;

                if (node.recvTime > node.sendTime) //收到了心跳回应
                {
                    totalTime_ += node.recvTime - node.sendTime;
                }
                else
                {
                    lossNum_++;
                }
            }
        }
    }

    std::pair<uint16_t, uint16_t> HeartManager::contentDecode(const std::string &content)
    {
        if (content.size() != 2 * sizeof(uint16_t))
            return {-1, -1};

        HeartData temp{0};

        std::copy(content.begin(), content.end(), temp.buf);

        return {temp.data.targetId_, temp.data.heartId_};
    }
    std::string HeartManager::contentEncode(uint16_t targetId, uint16_t heartId)
    {
        if (targetId <= 0 or heartId <= 0)
        {
            return std::string();
        }

        HeartData temp{0};

        temp.data.targetId_ = targetId;
        temp.data.heartId_ = heartId;

        return temp.buf;
    }
}