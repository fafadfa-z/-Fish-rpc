#include "rpc/protocol.h"
#include "string.h"
#include <cassert>
#include <stdexcept>

namespace Fish
{
    bool Protocol::setContent(const char *ptr, size_t len)
    {
        [[unlikely]]if(len<= Max_size - Base_length)
        {
            return false;
        }
        content_.resize(len);

        std::copy(content_.begin(),content_.end(),ptr);
    }

    bool Protocol::setContent(const std::deque& que)
    {
        [[unlikely]]if(que.size()<= Max_size - Base_length)
        {
            return false;
        }
        content_.assign(que.begin(),que.end());
    }

    bool Protocol::setContent(const char *ptr)
    {
        return setContent(ptr,::strlen(ptr));
    }

    bool Protocol::setContent(const std::string &str)
    {
        [[unlikely]]if(str.size()<= Max_size - Base_length)
        {
            return false;
        }
        content_ = str;
    }

    bool Protocol::setContent(const std::string_view view)
    {
        [[unlikely]]if(view.size()<= Max_size - Base_length)
        {
            return false;
        }
        
        content_ = view;
    }

    void Protocol::calcSize() //重新计算总长度
    {
        size_ = Base_length + content_.size();
    }


    static ptr createHealthPacket(); //创建心跳包

    static ptr createProvider(const std::string &); //创建注册成为Provider的包

    static ptr createConsumer(const std::string &); //创建注册成为Consumer的包

    ptr Protocol::create(MsgType type, uint16_t id)
    {
        ptr result = std::make_shared<Protocol>();

        result->setType(type);
        result->setId(id);
    }

}
