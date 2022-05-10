#pragma once

#include <functional>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <tuple>
#include <string>

#include <stdexcept>

namespace Fish::rpc
{
    // 类型及其id
    enum TypeId
    {
        // 聚合类型:
        Type_vector = 1,         // 1
        Type_list,               // 2
        Type_deque,              // 3
        Type_set,                // 4
        Type_multiset,           // 5
        Type_unordered_set,      // 6
        Type_unordered_multiset, // 7
        Type_pair,               // 8
        Type_map,                // 9
        Type_multimap,           // 10
        Type_unordered_map,      // 11
        Type_unordered_multimap, // 12
        Type_tuple,              // 13

        // 简单类型
        Type_int8 = 64, // 64
        Type_int16,     // 65
        Type_int32,     // 66
        Type_int64,     // 67
        Type_uint8,     // 68
        Type_uint16,    // 69
        Type_uint32,    // 70
        Type_uint64,    // 71
        Type_float,     // 72
        Type_double,    // 73
        Type_string,    // 74
    };

#if 1
    template <typename T>
    struct findtype
    {
        static constexpr int type = 0;
    };
    template <typename... Arg>
    struct findtype<std::vector<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_vector;

        using sub_type = std::vector<Arg...>::value_type;
    };

    template <typename... Arg>
    struct findtype<std::list<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_list;

        using sub_type = std::list<Arg...>::value_type;
    };

    template <typename... Arg>
    struct findtype<std::deque<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_deque;

        using sub_type = std::deque<Arg...>::value_type;
    };

    template <typename... Arg>
    struct findtype<std::set<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_set;

        using sub_type = std::set<Arg...>::key_type;
    };

    template <typename... Arg>
    struct findtype<std::multiset<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_multiset;

        using sub_type = std::multiset<Arg...>::key_type;
    };

    template <typename... Arg>
    struct findtype<std::unordered_set<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_unordered_set;

        using sub_type = std::unordered_set<Arg...>::key_type;
    };

    template <typename... Arg>
    struct findtype<std::unordered_multiset<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_unordered_multiset;

        using sub_type = std::unordered_multiset<Arg...>::key_type;
    };

    template <typename... Arg>
    struct findtype<std::map<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_map;

        using key_type = std::map<Arg...>::key_type;
        using value_type = std::map<Arg...>::mapped_type;
    };

    template <typename... Arg>
    struct findtype<std::multimap<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_multimap;

        using key_type = std::multimap<Arg...>::key_type;
        using value_type = std::multimap<Arg...>::mapped_type;
    };

    template <typename... Arg>
    struct findtype<std::unordered_map<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_unordered_map;

        using key_type = std::unordered_map<Arg...>::key_type;
        using value_type = std::unordered_map<Arg...>::mapped_type;
    };

    template <typename... Arg>
    struct findtype<std::unordered_multimap<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_unordered_multimap;

        using key_type = std::unordered_multimap<Arg...>::key_type;
        using value_type = std::unordered_multimap<Arg...>::mapped_type;
    };

    template <typename... Arg>
    struct findtype<std::pair<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_pair;

        using key_type = std::pair<Arg...>::first_type;
        using value_type = std::pair<Arg...>::second_type;
    };

    template <typename... Arg>
    struct findtype<std::tuple<Arg...>>
    {
        static constexpr uint8_t type = TypeId::Type_tuple;
    };

    template <>
    struct findtype<int8_t>
    {
        static constexpr uint8_t type = TypeId::Type_int8;
    };
    template <>
    struct findtype<int16_t>
    {
        static constexpr uint8_t type = TypeId::Type_int16;
    };
    template <>
    struct findtype<int32_t>
    {
        static constexpr uint8_t type = TypeId::Type_int32;
    };
    template <>
    struct findtype<int64_t>
    {
        static constexpr uint8_t type = TypeId::Type_int64;
    };
    template <>
    struct findtype<uint8_t>
    {
        static constexpr uint8_t type = TypeId::Type_uint8;
    };
    template <>
    struct findtype<uint16_t>
    {
        static constexpr uint8_t type = TypeId::Type_uint16;
    };
    template <>
    struct findtype<uint32_t>
    {
        static constexpr uint8_t type = TypeId::Type_uint32;
    };
    template <>
    struct findtype<uint64_t>
    {
        static constexpr uint8_t type = TypeId::Type_uint64;
    };
    template <>
    struct findtype<float>
    {
        static constexpr uint8_t type = TypeId::Type_float;
    };
    template <>
    struct findtype<double>
    {
        static constexpr uint8_t type = TypeId::Type_double;
    };

    template <>
    struct findtype<std::string>
    {
        static constexpr uint8_t type = TypeId::Type_string;
    };

#endif

    template <typename T>
    struct function_traits;

    template <typename ReturnType, typename... Args>
    struct function_traits<std::function<ReturnType(Args...)>>
    {

        static constexpr size_t nargs = sizeof...(Args);

        using return_type = ReturnType;

        template <size_t i>
        struct arg
        {
            typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
        };
    };

    template <typename Type>
    void findType(Type &t, std::vector<uint8_t> &paraMes)
    {
        if constexpr (findtype<Type>::type == 0)
        {
            throw std::exception();
        }

        //对于一个主要参数的类型(vector, set 等)
        if constexpr (findtype<Type>::type <= TypeId::Type_unordered_multiset)
        {
            typename findtype<Type>::sub_type sub;

            paraMes.push_back(findtype<Type>::type);

            findType(sub, paraMes);
        }

        //对于两个主要参数的类型(map, pair 等)
        if constexpr (findtype<Type>::type >= TypeId::Type_pair and findtype<Type>::type <= TypeId::Type_unordered_multimap)
        {
            typename findtype<Type>::key_type key;
            typename findtype<Type>::key_type value;

            paraMes.push_back(findtype<Type>::type);

            findType(key, paraMes);   // 查找第一个参数的类型
            findType(value, paraMes); // 查找第二个参数的类型
        }
        //简单类型
        if constexpr (findtype<Type>::type >= Type_int8)
        {
            paraMes.push_back(findtype<Type>::type);
        }
    }

#if 0
        函数序列化格式:
        
        name:${function_name_}\n
        return:&{return_type_}\n
        arg_size:&{arge_type_.size()}\n  
        arge_type_[0]\n
        arge_type_[1]\n
        ...
#endif
#include <iostream>

    using namespace std;

    class FunPacket //储存函数的调用信息
    {
    public:
    
        template<typename Function>
        void setFun(std::string name,std::function<Function>function)//获取函数的信息.
        {
            typename function_traits<Function>::return_type return_temp;
            findType(return_temp,return_type_);

            constexpr size_t size = function_traits<Function>::nargs;
            arge_type_.resize(size);
            
            for(int i=0;i<size;i++)
            { 
                typename function_traits<Function>::arg<i>::type temp;
                
                findType(temp,arge_type_[i]);
            }
            function_name_ = name;
        }

    private:
        std::vector<uint8_t> return_type_;
        std::vector<std::vector<uint8_t>> arge_type_;
        std::string function_name_;
    };

}
