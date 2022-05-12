#include "rpc/protocol.h"
#include "string.h"
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <chrono>

namespace Fish
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
        mes_.mes.size_ = Base_length + content_.size();
    }

    std::pair<bool, size_t> Protocol::create(std::string_view view)
    {
        if (view.size() == 0)
            return {false, 0};

        size_t index;

        switch (process_)
        {
        case ProtocolProcess::empty:
            //寻找对应的魔法数
            index = view.find(static_cast<char>(default_magic_));

            if (index == std::string_view::npos)
                return {false, view.size()};

            view = view.substr(index);

            if (view.size() < Base_length)
            {
                return {false, index}; //将魔法数读出来,
            }

            std::copy(view.data(), view.data() + Base_length, mes_.mesStr);

            process_ = ProtocolProcess::need_content;

            if (view.size() < mes_.mes.size_) //如果内容没有全部收到
            {
                return {false, index + Base_length};
            }

            view = view.substr(Base_length);

        case ProtocolProcess::need_content:
            if (content_.size() + view.size() < mes_.mes.size_)
            {
                content_ += view;

                return {false, view.size()};
            }

            view = view.substr(0, mes_.mes.size_ - content_.size());

            content_ += view;

            process_ = ProtocolProcess::perfect;

            return {true, view.size()};

        case ProtocolProcess::perfect:

            throw std::runtime_error("本不应该出现这种情况");
        };

        return {false, 0};
    }

    Protocol::ptr Protocol::createHealthPacket(uint16_t id) //创建心跳包
    {
        ptr result = std::make_shared<Protocol>();

        result->mes_ = result->setHeadMes(MsgType::Rpc_Health, id);
        result->content_ = getTime();

        result->calcSize();
        return result;
    }

    Protocol::ptr Protocol::createProvider(uint16_t id) //创建注册成为Provider的包
    {
        ptr result = std::make_shared<Protocol>();

        result->mes_ = result->setHeadMes(MsgType::Rpc_Provider, id);
        result->content_ = getTime();

        result->calcSize();
        return result;
    }

    Protocol::ptr Protocol::createConsumer(uint16_t id) //创建注册成为Consumer的包
    {
        ptr result = std::make_shared<Protocol>();

        result->mes_ = result->setHeadMes(MsgType::Rpc_Consumer, id);

        result->content_ = getTime();

        result->calcSize();
        return result;
    }

    Protocol::headMes Protocol::setHeadMes(MsgType type, uint16_t id)
    {
        headMes result;

        result.mes.magic_ = default_magic_;
        result.mes.version_ = default_version_;
        result.mes.type_ = type;
        result.mes.id_ = id;

        return result;
    }

    std::string Protocol::result()
    {
        std::string result;

        result.resize(Base_length);

        std::copy(mes_.mesStr, mes_.mesStr + Base_length,&result[0]);

        result += content_;

        return result;
    }

}
