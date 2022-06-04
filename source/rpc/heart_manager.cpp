#include "rpc/heart_manager.h"

namespace Fish
{

    std::optional<std::string> HeartManager::sendHeart()
    {
        if (!alive())
            return std::nullopt;



    }

    bool HeartManager::alive()
    {
        for(auto& node :list_ )
        {
            if(node.recvTime>node.sendTime) //只要有一个收到了，就没问题
                return true;
        }
    }

    void HeartManager::updateList(HeartManager::heatTask &&node)
    {

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
        }
    }

}