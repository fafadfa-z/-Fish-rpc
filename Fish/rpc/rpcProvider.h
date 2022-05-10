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
            methods_[name] = [func, this](const std::vector<const std::string> &arg, std::string &ret)
            {
                runMethod(func, serializer_, arg, ret);
            };
        }

        template <typename Fun>
        static void runMethod(Fun function, Serializer serializer, const std::vector<const std::string> &arg, std::string &ret)
        {






            
        }

    private:
        using MethodType = std::function<void(Serializer, const std::vector<const std::string> &)>;

        std::unordered_map<std::string, MethodType> methods_;

        Serializer serializer_;
    };
}