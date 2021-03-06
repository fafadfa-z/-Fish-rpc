#include "rpc/heart_manager.h"
#include "base/timer.h"

#include <cassert>

namespace Fish
{
    HeartManager::HeartManager(uint16_t self,uint16_t target,size_t maxSize)
        : maxSize_(maxSize),selfId_(self),targetId_(target)
    {

    }

    std::optional<std::string> HeartManager::sendHeart()
    {
        if (!alive())
            return std::nullopt;

        // auto &node = list_.back();

        // heatTask task{Timer::getNow_Milli(), 0, node.id + 1};

        // updateMessage();

        // insertNode(std::move(task));

    
        return "";

    }

    //目前只接受一个设备号和心跳id号
    void HeartManager::recvHeart(std::string& content)
    {
        assert(content.size() == sizeof(HeartData));

        HeartData temp;

        std::copy(content.begin(),content.end(),temp.buf);

        


    }


    bool HeartManager::alive()
    {
        for (auto &node : list_)
        {
            if (node.recvTime > node.sendTime) //只要有一个收到了，就没问题
                return true;
        }
        return false;
    }

    void HeartManager::insertNode(HeartManager::heatTask &&node)
    {
        [[unlikely]] if (list_.empty())  // 记录开始的时间
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
        for(auto& node : list_)
        {
            if(!node.statistics)
            {
                node.statistics = true;
                totalNum_ ++;

                if(node.recvTime > node.sendTime) //收到了心跳回应
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
}