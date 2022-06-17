#pragma once

/*
    @brief设计一个路由策略，实现请求方对不同服务方的调用
*/

#include<vector>
#include<unordered_map>
#include<string_view>
#include<queue>
#include<mutex>

#include"registry/rpc_registry.h"

namespace Fish
{
    /**
      *  @brief：在queue中，存储服务提供商的地址信息
    **/    
   
    

    enum RouteAlgorithm
    {
        //轮询算法
        POLLING
    };

    template<class T>
    class RouteStrategy
    {
        public:
            using ptr = std::shared_ptr<RouteStrategy>;
            virtual ~RouteStrategy();
            virtual auto& select(std::vector<T>& msgBus) = 0;

            virtual Fish::NodeStatus& checkStatus(T& );
        private:    
            std::unordered_map<uint16_t,ptr> prvdMap;                        
    };



    template<class T>
    class RollingStrategy : public RouteStrategy
    {        
        using ptr = std::shared_ptr<T>;
        public:
            ptr& select(std::vector<ptr>& msgBus)
            {
                std::lock_guard<std::mutex> lock(mtx_);
                while (!msgBus.empty())
                {
                    if(idx_ > msgBus.size()) idx_ = 0;
                    auto prvd = msgBus[idx_++];
                    if(checkStatus(prvd))
                        return prvd;                    
                }
                return nullptr;
            }

            bool checkStatus(ptr& prvd)
            {
                if(prvd->NodeStatus == NodeStatus::WORKING)
                return false;
                return true;
            }
            
        private:
            std::mutex mtx_;
            int idx_ = 0;
            std::unordered_map<std::string_view,ptr> prvdMap;

    };


    /**
      *  @brief：路由引擎，对外接口，通过不同的ra，选择不同的调用provider方法
    **/    
   
   template<class T>
   class RoutiEgine {
       public:
            static typename RouteStrategy<T>::ptr  routeMethod(RouteAlgorithm ra)
            {
                switch (ra)
                {
                case RouteAlgorithm::POLLING
                    return pollingStrategy_;
                default:
                    break;
                }
            }

       private:
            static typename RouteStrategy<T>::ptr pollingStrategy_;
   };

template<class T>
typename  RouteStrategy<T>::ptr RoutiEgine<T>::pollingStrategy_ = std::make_shared<RollingStrategy<T>>();
}