#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <limits.h>
#include "base/sync/mutex.h"

inline thread_local unsigned int thread_loop_id = UINT_MAX;

namespace Fish
{
    class Loop
    {   
        public:

        using MutexType = Mutex;

        Loop() = default;
        Loop(std::function<void()>);

        void setTask(std::function<void()> task);
        void setInitTask(std::function<void()> task)
        {
            initTask_ = task;
        }

        unsigned int loopId()const {return thread_loop_id;}

        void beginInCurrentThread(); //在当前线程开始loop

        std::thread beginInNewThread();  //在新的线程开始loop

        void closeLoop(); // 结束loop

        void addTempTask(std::function<void()>);
        

    private:
        std::atomic<bool> flag_;
        
        std::vector<std::function<void()>>tempTasks_;
        MutexType tempTaskMut_;

        
        std::function<void()> initTask_;
        std::function<void()> task_;

    };







}