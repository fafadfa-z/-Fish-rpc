#include "base/loop.h"
#include <iostream>
#include <stdexcept>

namespace Fish
{
    int getLooopId()
    {
        static unsigned int idSum = 0;

        return ++idSum;
    }

    Loop::Loop(std::function<void()> task)
        : flag_(false), task_(task)
    {

    }

    void Loop::beginInCurrentThread()
    {
        if (thread_loop_id != UINT_MAX)
            throw std::runtime_error("Loop: current thread already has a loop");

        if (!task_)
            throw std::runtime_error("Loop: haven't got task");

        if (flag_)
            throw std::runtime_error("Loop: looping now");

        tempTasks_.reserve(8);  

        thread_loop_id = getLooopId();

        flag_ = true;

        if(initTask_) initTask_();

        while (flag_)
        {
            task_();

            std::vector<std::function<void()>> temp;
            {
                MutexType::guard guard(tempTaskMut_);
                
                std::swap(temp,tempTasks_);

                tempTasks_.reserve(8);
            }

            for(auto& task : temp)
            {
                task();
            }
        }
        
        std::cout<<"Loop: "<<thread_loop_id<<" exit..." << std::endl;
    }

    std::thread Loop::beginInNewThread()
    {
        std::thread loopThread = std::thread([this]
                                             { beginInCurrentThread(); });

        return loopThread;
    }

    void Loop::setTask(std::function<void()> task)
    {   
        if(flag_)
            throw std::runtime_error("Loop: Please close the current task first");

        task_ = task;
    }

    void Loop::closeLoop()
    {
        flag_ = false;
    }

    void Loop::addTempTask(std::function<void()> task)
    {
        MutexType::guard guard(tempTaskMut_);
    
        tempTasks_.push_back(std::move(task));
    }


}
