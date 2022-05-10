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

        template <typename T>  //这里要是使用模板会生成一些奇怪的类型，
        bool input(T &t)
        {
            if constexpr (std::is_same_v<T, bool>)
            {
                buf_.input_int8(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, int8_t>)
            {
                buf_.input_int8(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, uint8_t>)
            {
                buf_.input_uint8(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, int16_t>)
            {
                buf_.input_int16(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, uint16_t>)
            {
                buf_.input_uint16(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, int32_t>)
            {
                buf_.input_int32(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, uint32_t>)
            {
                buf_.input_uint32(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, int64_t>)
            {
                buf_.input_int64(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, uint64_t>)
            {
                buf_.input_uint64(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                buf_.input_float(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                buf_.input_double(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, char *>)
            {
                buf_.input_str(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                buf_.input_string(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, const char *>)
            {
                buf_.input_str(t);
                return true;
            }
            else if constexpr (std::is_same_v<T, std::string_view>)
            {
                buf_.input_string_view(t);
                return true;
            }
            return false;
        }
        
        template <typename T>
        bool output(T &t)
        {
            if constexpr (std::is_same_v<T, bool>)
            {
                t= buf_.output_int8();
                return true;
            }
            else if constexpr (std::is_same_v<T, int8_t>)
            {
                t= buf_.output_int8();
                return true;
            }
            else if constexpr (std::is_same_v<T, uint8_t>)
            {
                t = buf_.output_uint8();
                return true;
            }
            else if constexpr (std::is_same_v<T, int16_t>)
            {
                t = buf_.output_int16();
                return true;
            }
            else if constexpr (std::is_same_v<T, uint16_t>)
            {
                t = buf_.output_uint16();
                return true;
            }
            else if constexpr (std::is_same_v<T, int32_t>)
            {
                t = buf_.output_int32();
                return true;
            }
            else if constexpr (std::is_same_v<T, uint32_t>)
            {
                t = buf_.output_uint32();
                return true;
            }
            else if constexpr (std::is_same_v<T, int64_t>)
            {
                t = buf_.output_int64();
                return true;
            }
            else if constexpr (std::is_same_v<T, uint64_t>)
            {
                t = buf_.output_uint64();
                return true;
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                t = buf_.output_float();
                return true;
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                t = buf_.output_double();
                return true;
            }            
            else if constexpr (std::is_same_v<T, std::string>)
            {
                t = buf_.output_string();
                return true;
            }
            return false;
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
    private:
        SeriBuf buf_;
    };

}
