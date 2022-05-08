#pragma once

#include <deque>
#include <string>
#include <string.h>
#include <string_view>
#include <stdexcept>
#include <cassert>

namespace Fish::rpc
{
    class SeriBuf  //序列化之后的buffer 
    {
    public:
        SeriBuf() = default;

        void input(const char *buf, unsigned long len) //将外面的数据输入到数组里。
        {
            const char *buf_end = buf + len;

            buf_.insert(buf_.end(), buf, buf_end);
        }

        void output(char *buf, unsigned long len) //将数据输出到外面
        {
            [[unlikely]] if (len > buf_.size())
            {
                throw std::out_of_range("Haven't enough len");
            }

            auto iter_begin = buf_.end() - len;

            std::copy(iter_begin, buf_.end(), buf);

            buf_.erase(iter_begin, buf_.end());
        }

        template <typename T>
        void inputTemplate(T value)
        {
            input(reinterpret_cast<char *>(&value), sizeof(value));
        }

        template <typename T>
        void outputTemplate(T &value)
        {
            output(reinterpret_cast<char *>(&value), sizeof(value));
        }

    private:
        std::deque<char> buf_;
    };

    template <>
    void SeriBuf::inputTemplate<const char *>(const char *value)
    {
        auto size = ::strlen(value);
        inputTemplate(size);
        input(value, size);
    }

    template <>
    void SeriBuf::inputTemplate<std::string>(const std::string value)
    {
        inputTemplate(value.size());
        input(value.c_str(), value.size());
    }

    template <>
    void SeriBuf::inputTemplate<std::string_view>(const std::string_view value)
    {
        inputTemplate(value.size());
        input(value.data(), value.size());
    }

    // template <>
    // void SeriBuf::outputTemplate<char *>(char *value)
    // {
    //     size_t size;

    //     outputTemplate(size);

    //     output(value, size);
    // }

    template <>
    void SeriBuf::outputTemplate<std::string &>(std::string &value)
    {
        size_t size;
        outputTemplate(size);

        value.resize(size);

        output(&value[0], value.size());
    }

}
