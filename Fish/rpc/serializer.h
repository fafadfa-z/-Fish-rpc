#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <list>
#include <vector>
#include <deque>


#include "serialize_buf.h"

// 序号  总大小    类型

//          8 位0表示函数名, 1,2,3 表示第 1 . 2. 3  个参数

namespace Fish::rpc
{
    // rpc 序列化/反序列化封装
    class Serializer
    {
    public:
        using ptr = std::shared_ptr<Serializer>;

        Serializer() = default;

        bool Serialize();

        template <typename T>
        void input(T &t)
        {
            buf_.inputTemplate(t);
        }

        template <typename T>
        void output(T &t)
        {
            buf_.outputTemplate(t);
        }

        template <typename T>
        [[maybe_unused]] Serializer &operator>>(T &i)
        {
            output(i);
            return *this;
        }

        template <typename T>
        [[maybe_unused]] Serializer &operator<<(const T &i)
        {
            input(i);
            return *this;
        }

        template <typename T>
        Serializer &operator>>(std::list<T> &v)
        {
            size_t size;
            output(size);
            for (size_t i = 0; i < size; ++i)
            {
                T t;
                
                this->operator>>(t);
                
                v.push_back(std::move(t));
            }
            return *this;
        }

        template <typename T>
        Serializer &operator<<(const std::list<T> &v)
        {
            input(v.size());
            for (auto &t : v)
            {
                this->operator<<(t);
            }
            return *this;
        }

        template <typename T>
        Serializer &operator>>(std::vector<T> &v)
        {
            size_t size;
            output(size);
            for (size_t i = 0; i < size; ++i)
            {
                T t;
                this->operator>>(t);
                v.push_back(std::move(t));
            }
            return *this;
        }

        template <typename T>
        Serializer &operator<<(const std::vector<T> &v)
        {
            input(v.size());
            for (auto &t : v)
            {
                this->operator<<(t);
            }
            return *this;
        }

        template <typename T>
        Serializer &operator>>(std::deque<T> &v)
        {
            size_t size;
            output(size);
            for (size_t i = 0; i < size; ++i)
            {
                T t;
                this->operator>>(t);
                v.push_back(std::move(t));
            }
            return *this;
        }

        template <typename T>
        Serializer &operator<<(const std::deque<T> &v)
        {
            input(v.size());
            for (auto &t : v)
            {
                this->operator<<(t);
            }
            return *this;
        }


        // template <typename T>
        // void input(T &t)
        // {
        //     if constexpr (std::is_same_v<T, bool>)
        //     {
        //         buf_.input_int8(t);
        //     }
        //     else if constexpr (std::is_same_v<T, int8_t>)
        //     {
        //         buf_.input_int8(t);
        //     }
        //     else if constexpr (std::is_same_v<T, uint8_t>)
        //     {
        //         buf_.input_uint8(t);
        //     }
        //     else if constexpr (std::is_same_v<T, int16_t>)
        //     {
        //         buf_.input_int16(t);
        //     }
        //     else if constexpr (std::is_same_v<T, uint16_t>)
        //     {
        //         buf_.input_uint16(t);
        //     }
        //     else if constexpr (std::is_same_v<T, int32_t>)
        //     {
        //         buf_.input_int32(t);
        //     }
        //     else if constexpr (std::is_same_v<T, uint32_t>)
        //     {
        //         buf_.input_uint32(t);
        //     }
        //     else if constexpr (std::is_same_v<T, int64_t>)
        //     {
        //         buf_.input_int64(t);
        //     }
        //     else if constexpr (std::is_same_v<T, uint64_t>)
        //     {
        //         buf_.input_uint64(t);
        //     }
        //     else if constexpr (std::is_same_v<T, float>)
        //     {
        //         buf_.input_float(t);
        //     }
        //     else if constexpr (std::is_same_v<T, double>)
        //     {
        //         buf_.input_double(t);
        //     }
        //     else if constexpr (std::is_same_v<T, char *>)
        //     {
        //         buf_.input_str(t);
        //     }
        //     else if constexpr (std::is_same_v<T, std::string>)
        //     {
        //         buf_.input_string(t);
        //     }
        //     else if constexpr (std::is_same_v<T, const char *>)
        //     {
        //         buf_.input_str(t);
        //     }
        //     else if constexpr (std::is_same_v<T, std::string_view>)
        //     {
        //         buf_.input_string_view(t);
        //     }
        // }

    private:
        SeriBuf buf_;
    };

}
