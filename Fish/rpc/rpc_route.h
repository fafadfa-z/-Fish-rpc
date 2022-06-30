#pragma once

/*
    @brief设计一个路由策略，实现请求方对不同服务方的调用
*/

#include<vector>
#include<unordered_map>
#include<string_view>
#include<queue>
#include<mutex>
#include"error.h"

#include"registry/rpc_registry.h"
#include"rpc/external_node.h"

namespace Fish
{
    /**
      *  @brief：在queue中，存储服务提供商的地址信息
    **/    
   
    

    enum RouteAlgorithm
    {
        //轮询算法
        POLLING,
        //新机上线
        TESTNODE
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


    template<class T>
    class TestnodeStrategy:public RouteStrategy
    {
        using ptr = std::shared_ptr<T>;

        public:
        /**
         * @brief 既然是测试，是不是应该所有节点都测试一遍？还是把这个操作权交给用户
         * 
         * @param msgBus 
         * @return ptr& 
         */
            ptr& select(std::vector<ptr>& msgBus)
            {
                std::lock_guard<std::mutex> lock(mtx_);
                for(int i = 0;i < msgBus.size();i++)
                {
                    auto cusNode = msgBus[i];
                    if(customerMap_.count(cusNode))
                    {
                        for(auto i : testNodeMap_)
                        {
                            checkStatus(i.first);
                            return i.first;
                        }
                    }
                }
            }


            /**
             * @brief 当新上线需要调试的provider，添加需要调试的节点，并设置可以访问调试节点的
             *         customer.函数返回当前的provider节点数目
             * 
             * @param testNode provider
             * @param op    true->add, false->delete
             * @return int 
             */

            int editProviderNode(ptr& testNode,bool op)
            {
                if(op)
                {
                    if(!testNodemap_.count(testNode))
                    {
                        testNodeMap_[testNode] = idx_++;
                        testNodeMap_cnt++;
                    }
                }
                else{
                    if(testNodeMap_cnt == 0)
                        throw std::runtime_error("operation wrong! testNodemap is empty.");
                    if(testNodeMap_.count(testNode))
                    {
                        testNodeMap_.erase(testNode);
                        testNodeMap_cnt--;
                    }
                }                
                return testNodeMap_cnt;                
            }

            /**
             * @brief 同上
             * 
             * @param testNode 
             * @param op 
             * @return int 
             */
            int editConsumerNode(ptr& testNode,bool op)
            {
                if(op)
                {
                    if(!customerMap_.count(testNode))
                    {
                        customerMap_[testNode]++;
                        customerMap_cnt++;
                    }
                }
                else{
                    if(customerMap_cnt == 0)
                         throw std::runtime_error("operation wrong! customerNodemap is empty.");
                    if(customerMap_.count(testNode))
                    {
                        customerMap_.erase(testNode);
                        customerMap_cnt--;
                    }
                }
                return customerMap_cnt;
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
            int customerMap_cnt = 0;
            std::unordered_map<std::string_view,ptr> customerMap_;
            int testNodeMap_cnt = 0;
            std::unordered_map<ptr,int> testNodeMap_;
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
            static typename RouteStrategy<T>::ptr testnodeStratety_;
   };

template<class T>
typename  RouteStrategy<T>::ptr RoutiEgine<T>::pollingStrategy_ = std::make_shared<RollingStrategy<T>>();

template<class T>
typename  RouteStrategy<T>::ptr RoutiEgine<T>::testnodeStratety_ = std::make_shared<TestnodeStrategy<T>>();
}