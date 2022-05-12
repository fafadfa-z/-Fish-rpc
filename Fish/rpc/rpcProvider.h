#pragma once

#include <memory>
#include "base/sync/mutex.h"
#include "net/tcp_server.h"
#include "serializer.h"
#include "unordered_map"
#include "functional"

#include "rpc.h"

namespace Fish::rpc
{
    class RpcProvider : public TcpServer
    {

    public:
        RpcProvider(TcpAddr addr, std::string name = "rpc server");

        template <typename Func>
        void registerMethod(const std::string &name, std::function<Func> func)
        {
            auto lambda = [func](Serializer &arg, Serializer &ret)
            {
                runMethod(func, arg, ret);
            };

            methods_[name] = std::move(lambda);
        }

        void run(const std::string &name, Serializer &arg, Serializer &ret)
        {
            auto &fun = methods_[name];
            fun(arg, ret);
        }

    private:
        template <typename Fun>
        static void runMethod(Fun function, Serializer &arg_ser, Serializer &ret_ser)
        {
            // FunPacket packet;

            // packet.setFun(function);

            using Args = typename function_traits<Fun>::tuple_type;

            Args args;
            arg_ser >> args;

            constexpr auto size = std::tuple_size<Args>::value;

            auto invoke = [&function, &args ]<std::size_t... Index>(std::index_sequence<Index...>)
            {
                return function(std::get<Index>(std::forward<Args>(args))...);
            };

            if constexpr (std::is_same_v<typename function_traits<Fun>::return_type, void>)
            {
                invoke(std::make_index_sequence<size>{});
            }
            else
            {
                typename function_traits<Fun>::return_type rt = invoke(std::make_index_sequence<size>{});

                ret_ser << rt;
            }
        }

    private:
        using MethodType = std::function<void(Serializer &, Serializer &)>;

        std::unordered_map<std::string, MethodType> methods_;

        Serializer serializer_;
    };
}