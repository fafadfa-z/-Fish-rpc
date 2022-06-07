#pragma once

#include <stdint.h>
#include <memory>
#include <atomic>
#include <functional>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "rpc/heart_manager.h"
#include "base/sync/mutex.h"
#include "base/non_copyable.h"


namespace Fish
{
    enum NodeStatus // provider 的状态
    {
        health,
        weak,
        dead
    };

    class Channel;

    class ExternNode : public HeartManager
    {
    public:
        using ptr = std::shared_ptr<ExternNode>;

        ExternNode(uint16_t self, uint16_t target, size_t maxHeart = 3)
            : HeartManager(self, target, maxHeart)
        {
        }

        virtual ~ExternNode() = default;


        void sendMeg(std::string_view);

    protected:


        NodeStatus status_;

        std::weak_ptr<Channel> channel_;
    };


    class ProviderNode : public ExternNode
    {
    public:
        using ptr = std::shared_ptr<ExternNode>;

        ProviderNode(uint16_t self, uint16_t target, size_t maxHeart = 3)
            :ExternNode(self, target, maxHeart)
        {}



    private:
        std::unordered_set<std::string> mathods_; //储存providered支持的方法。
    };


    class NodeManager :public NonCopyable
    {
    public:
        using NodePtr = std::shared_ptr<ProviderNode>;

        NodeManager() = default;

        void setEraseCallBack(auto cb){eraseCallBack_ = cb;}



        uint16_t createId() { return ++currentId_; }


        bool addNode(NodePtr);    //添加节点

        void addInTimer_heart(uint16_t);  //在定时器中跑的心跳检测函数

    private:
        void eraseNode(NodePtr);  //删除节点

        std::atomic<uint16_t> currentId_ = 0;

        std::function<void(uint16_t)> eraseCallBack_;

        Mutex nodeGuard_;
        std::unordered_map<uint16_t, NodePtr> nodes_;

        size_t size = 0;

    };

}