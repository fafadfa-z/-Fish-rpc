#include "rpc/protocol.h"
#include "string.h"
#include <cassert>
#include <stdexcept>
#include <chrono>

namespace Fish::rpc
{
    std::string getTime()
    {
        using namespace std::chrono;

        auto temp = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

        std::string result;

        result.resize(sizeof(temp));

        ::memcpy(&(result[0]), &temp, sizeof(temp));

        return result;
    }

    bool Protocol::setContent(const char *ptr, size_t len)
    {
        [[unlikely]] if (len <= Max_size - Base_length)
        {
            return false;
        }
        content_.resize(len);

        std::copy(ptr, ptr + len, content_.begin());

        return true;
    }

    bool Protocol::setContent(const std::deque<char> &que)
    {
        [[unlikely]] if (que.size() <= Max_size - Base_length)
        {
            return false;
        }
        content_.assign(que.begin(), que.end());
        return true;
    }

    bool Protocol::setContent(const char *ptr)
    {
        return setContent(ptr, ::strlen(ptr));
    }

    bool Protocol::setContent(const std::string &str)
    {
        [[unlikely]] if (str.size() <= Max_size - Base_length)
        {
            return false;
        }
        content_ = str;
        return true;
    }

    bool Protocol::setContent(const std::string_view view)
    {
        [[unlikely]] if (view.size() <= Max_size - Base_length)
        {
            return false;
        }

        content_ = view;
        return true;
    }

    void Protocol::calcSize() //重新计算总长度
    {
        size_ = Base_length + content_.size();
    }

    Protocol::ptr Protocol::createHealthPacket(uint16_t id) //创建心跳包
    {
        ptr result = std::make_shared<Protocol>();

        result->type_ = MsgType::Rpc_Health;
        result->id_ = id;

        result->content_ = getTime();

        result->calcSize();
        return result;
    }

    Protocol::ptr Protocol::createProvider(uint16_t id) //创建注册成为Provider的包
    {
        ptr result = std::make_shared<Protocol>();

        result->type_ = MsgType::Rpc_Provider;
        result->id_ = id;

        result->content_ = getTime();

        result->calcSize();
        return result;
    }

    Protocol::ptr Protocol::createConsumer(uint16_t id) //创建注册成为Consumer的包
    {
        ptr result = std::make_shared<Protocol>();

        result->type_ = MsgType::Rpc_Consumer;
        result->id_ = id;

        result->content_ = getTime();

        result->calcSize();
        return result;
    }

    Protocol::ptr Protocol::create(MsgType type, uint16_t id)
    {
        ptr result = std::make_shared<Protocol>();

        result->setType(type);
        result->setId(id);

        return result;
    }

}
