#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <limits.h>
#include <string>
#include "base/sync/mutex.h"
#include "base/non_copyable.h"

inline thread_local unsigned int thread_loop_id = UINT_MAX;

namespace Fish
{
    class Loop : public NonCopyable
    {   
        public:

        using MutexType = Mutex;

        Loop(std::string name= "") :name_(name){}
        Loop(std::function<void()>);

        ~Loop(){closeLoop();}

        void setTask(std::function<void()> task);
        void setInitTask(std::function<void()> task)
        {
            initTask_ = task;
        }

        unsigned int loopId()const {return thread_loop_id;}

        void beginInCurrentThread(); //在当前线程开始loop

        std::thread beginInNewThread();  //在新的线程开始loop

        void closeLoop(); // 结束loop

        void addTempTask(std::function<void()>); //增加临时任务


        const std::string& name()const {return name_;}

        void setName(const std::string& name){name_ = name;}
        

    private:
        std::atomic<bool> flag_;
        
        std::vector<std::function<void()>>tempTasks_;
        MutexType tempTaskMut_;

        
        std::function<void()> initTask_;
        std::function<void()> task_;


        std::string name_;

    };







}