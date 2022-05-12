#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "serialize_buf.h"

// 序列化 目前支持标准STL容器
// vector list deque 
// string
// map multimap unordered_map unordered_multimap
// set multiset unordered_set unordered_multiset
// pair tuple

// 支持基本数据类型：
// int8  int16  int32  int64
// uint8 uint16 uint32 uint64
// float double
// bool

// 及容器-容器 和 容器-基本数据类型 的组合


namespace Fish
{
    // rpc 序列化/反序列化封装
    class Serializer
    {
    public:
        using ptr = std::shared_ptr<Serializer>;

        Serializer() = default;

        void setBuf(const std::string &str) { buf_ = str; }

        void clear() { buf_.clear(); }

        const std::deque<char> &buf() const
        {
            return buf_.buf();
        }

        template <typename T> //这里要是使用模板会生成一些奇怪的类型，
        bool input(const T &t)
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
                t = buf_.output_int8();
                return true;
            }
            else if constexpr (std::is_same_v<T, int8_t>)
            {
                t = buf_.output_int8();
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
            v.resize(size);
            for (size_t i = 0; i < size; ++i)
            {
                T t;
                this->operator>>(t);
                v[i] = std::move(t);
            }
            return *this;
        }

        template <typename T>
        Serializer &operator<<(const std::vector<T> &v)
        {
            size_t size = v.size();
            input(size);
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

        template <typename T>
        Serializer &operator>>(std::set<T> &v)
        {
            size_t size;
            output(size);

            for (size_t i = 0; i < size; ++i)
            {
                T t;
                output(t);
                v.insert(std::move(t));
            }
            return *this;
        }

        template <typename T>
        Serializer &operator<<(const std::set<T> &v)
        {
            input(v.size());
            for (auto &t : v)
            {
                (*this) << t;
            }
            return *this;
        }

        template <typename T>
        Serializer &operator>>(std::multiset<T> &v)
        {
            size_t size;
            output(size);

            for (size_t i = 0; i < size; ++i)
            {
                T t;
                output(t);
                v.insert(std::move(t));
            }
            return *this;
        }

        template <typename T>
        Serializer &operator<<(const std::multiset<T> &v)
        {
            input(v.size());
            for (auto &t : v)
            {
                (*this) << t;
            }
            return *this;
        }

        template <typename T>
        Serializer &operator>>(std::unordered_set<T> &v)
        {
            size_t size;
            output(size);

            for (size_t i = 0; i < size; ++i)
            {
                T t;
                output(t);
                v.insert(std::move(t));
            }
            return *this;
        }

        template <typename T>
        Serializer &operator<<(const std::unordered_set<T> &v)
        {
            input(v.size());
            for (auto &t : v)
            {
                (*this) << t;
            }
            return *this;
        }

        template <typename T>
        Serializer &operator>>(std::unordered_multiset<T> &v)
        {
            size_t size;
            output(size);

            for (size_t i = 0; i < size; ++i)
            {
                T t;
                output(t);
                v.insert(std::move(t));
            }
            return *this;
        }

        template <typename T>
        Serializer &operator<<(const std::unordered_multiset<T> &v)
        {
            input(v.size());
            for (auto &t : v)
            {
                (*this) << t;
            }
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator<<(const std::pair<K, V> &m)
        {
            (*this) << m.first << m.second;
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator>>(std::pair<K, V> &m)
        {
            (*this) >> m.first >> m.second;
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator>>(std::map<K, V> &m)
        {
            size_t size;
            output(size);
            for (size_t i = 0; i < size; ++i)
            {
                std::pair<K, V> p;
                (*this) >> p;
                m.insert(std::move(p));
            }
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator<<(const std::map<K, V> &m)
        {
            input(m.size());
            for (auto &t : m)
            {
                (*this) << t;
            }
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator>>(std::unordered_map<K, V> &m)
        {
            size_t size;
            output(size);
            for (size_t i = 0; i < size; ++i)
            {
                std::pair<K, V> p;
                (*this) >> p;
                m.insert(std::move(p));
            }
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator<<(const std::unordered_map<K, V> &m)
        {
            input(m.size());
            for (auto &t : m)
            {
                (*this) << t;
            }
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator>>(std::multimap<K, V> &m)
        {
            size_t size;
            output(size);
            for (size_t i = 0; i < size; ++i)
            {
                std::pair<K, V> p;
                (*this) >> p;
                m.insert(std::move(p));
            }
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator<<(const std::multimap<K, V> &m)
        {
            input(m.size());
            for (auto &t : m)
            {
                (*this) << t;
            }
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator>>(std::unordered_multimap<K, V> &m)
        {
            size_t size;
            output(size);
            for (size_t i = 0; i < size; ++i)
            {
                std::pair<K, V> p;
                (*this) >> p;
                m.insert(std::move(p));
            }
            return *this;
        }

        template <typename K, typename V>
        Serializer &operator<<(const std::unordered_multimap<K, V> &m)
        {
            input(m.size());
            for (auto &t : m)
            {
                (*this) << t;
            }
            return *this;
        }

        template <typename... Args>
        Serializer &operator>>(std::tuple<Args...> &t)
        {

            const auto &deserializer = [this]<typename Tuple, std::size_t... Index>(Tuple & t, std::index_sequence<Index...>)
            {
                (void)((*this) >> ... >> std::get<Index>(t));
            };
            deserializer(t, std::index_sequence_for<Args...>{});
            return *this;
        }

        template <typename... Args>
        Serializer &operator<<(const std::tuple<Args...> &t)
        {

            const auto &package = [this]<typename Tuple, std::size_t... Index>(const Tuple &t, std::index_sequence<Index...>)
            {
                (void)((*this) << ... << std::get<Index>(t));
            };

            package(t, std::index_sequence_for<Args...>{});
            return *this;
        }

    private:
        SeriBuf buf_;
    };

}
