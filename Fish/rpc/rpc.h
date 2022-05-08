#pragma once

#include <functional>

template<typename T>
struct function_traits;

template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(Args...)>
{

    static const size_t nargs = sizeof...(Args);

    using return_type = ReturnType;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

namespace Fish
{
    template<typename Fun>
    void funProxy(Fun fun)
    {
        using Return = typename function_traits<Fun>::return_type;
    
        constexpr size_t ArgSize = function_traits<Fun>::nargs;

        






    }

}



