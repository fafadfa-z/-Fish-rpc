#include "rpc/protocol_manager.h"
#include "net/channel.h"

#include <mutex>

namespace Fish
{
    Protocol::ptr ProtocolManager::readMes(std::shared_ptr<Channel>& channel)
    {

        auto view = channel->dispReadBuf();

        bool oldFlag = false;

        Protocol::ptr protocol;

        {
            std::shared_lock<std::shared_mutex> guard(protocalMut_);

            auto itor = undoProtols_.find(channel->fd());

            if (itor == undoProtols_.end())
                protocol = std::make_shared<Protocol>();
            else
            {
                protocol = itor->second;
                oldFlag = true;
            }
        }

        auto [flag, size] = protocol->create(view);

        channel->eraseRead(size);

        if (flag and oldFlag) //如果上一次处理的数据完成了。
        {
            std::unique_lock<std::shared_mutex> guard(protocalMut_);

            undoProtols_.erase(undoProtols_.find(channel->fd()));
        }
        else if ((!flag) and (!oldFlag)) //如果来了新的，未完成的连接
        {
            std::unique_lock<std::shared_mutex> guard(protocalMut_);

            undoProtols_.emplace(channel->fd(), protocol);
        }
    
        if(flag)
            return protocol;

        return Protocol::ptr();
    }
}