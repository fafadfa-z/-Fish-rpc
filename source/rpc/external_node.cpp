#include "rpc/external_node.h"

#include "rpc/protocol_manager.h"

#include "net/channel.h"

#include <cstdint>
#include <cassert>
#include <unordered_map>


#include <iostream>
using namespace std;


namespace Fish
{

    void ExternNode::sendMeg(std::string_view view)
    {
        auto channel = channel_.lock();

        if (channel)
            channel->send(view);
    }

    bool NodeManager::addNode(NodePtr node)
    {
        auto id = node->getTargetId();

        LockGuard guard(nodeGuard_);

        auto [iter, flag] = nodes_.insert(std::pair(id, node));

        if (flag == false)
            return false;

        return true;
    }

    void NodeManager::eraseNode(NodePtr node)
    {
        cout<<"心跳检测失败 id = "<<node->getId()<<endl;

        auto id = node->getTargetId();

        {
            LockGuard guard(nodeGuard_);

            auto iter = nodes_.find(id);

            assert(iter != nodes_.end());

            nodes_.erase(iter);
        }

        if (eraseCallBack_)
            eraseCallBack_(id);
    }



    void NodeManager::addInTimer_heart(uint16_t id)
    {
        std::unordered_map<uint16_t, NodePtr>::iterator iter;

        {
            LockGuard guard(nodeGuard_);

            iter = nodes_.find(id);
        }

        assert(iter != nodes_.end());

        auto content = iter->second->sendHeart();

        if (content == std::nullopt) //如果心跳检测失败，删除节点。
            eraseNode(iter->second);
        else
        {
            auto protocol = ProtocolManager::createHeart(iter->second->getSelfId(), *content);

  

            auto mes = protocol->result();
            iter->second->sendMeg(mes);

            
        }

        
    }

    void NodeManager::heartRecv(uint16_t id, const std::string& content)
    {
        std::unordered_map<uint16_t, NodePtr>::iterator iter;

        {
            LockGuard guard(nodeGuard_);

            iter = nodes_.find(id);
        }
        assert(iter != nodes_.end());

        iter->second->recvHeart(content);
    }

}
