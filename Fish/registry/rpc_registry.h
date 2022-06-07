#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "net/tcp_addr.h"

#include "base/sync/mutex.h"
#include "net/tcp_server.h"
#include "rpc/protocol_manager.h"
#include "rpc/heart_manager.h"

namespace Fish
{
    enum ProviderStatus  //provider 的状态
    {
        HEALTH, 
        WEAK,
        BUSY,
        DEAD
    };

    struct ProviderMsg //用于储存 服务提供方的所有信息
    {
        using ptr = std::shared_ptr<ProviderMsg>;

        uint16_t id;  

        TcpAddr addr;

        ProviderStatus status;

        std::weak_ptr<Channel> channel;

        int64_t lastHeart_send;  //上一次心跳检测发送的时间  
        int64_t lastHeart_recv;  //上一次心跳检测接收的时间
    };

    class Channel;

    class RpcRegistry : public TcpServer
    {
    public:
        RpcRegistry(TcpAddr addr, std::string name = "rpc register");

        void begin() override;

        void handleMessage(std::shared_ptr<Channel>);


        const std::string &name() { return name_; }


        // void sendMes(std::shared_ptr<Channel>);
    private:
        
        Mutex providers_mut_;
        std::vector<ProviderMsg> providers_;

        // 处理provider信息
        void handleProvider(Protocol::ptr&,std::shared_ptr<Channel>&);

        // 处理新连接的 provider
        void providerNew(Protocol::ptr&,std::shared_ptr<Channel>&);

    private:
        void healthDetection(uint16_t);


    private:
        
        ProtocolManager protocols_;
    
        std::string name_;
    };

}
