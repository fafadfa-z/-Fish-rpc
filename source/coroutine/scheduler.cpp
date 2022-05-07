#include "coroutine/scheduler.h"

#include <unistd.h>
#include <sys/syscall.h>


#include <cassert>
#include <iostream>

#include <signal.h>

namespace Fish
{
    static thread_local Scheduler *t_scheduler = nullptr;

    auto GetThreadId()  // 获取线程ID
    {
        return syscall(SYS_gettid);
    }

    Scheduler::Scheduler(size_t threads, const std::string &name)
        : m_name(name), m_threadCount(threads)
    {
        assert(t_scheduler == nullptr); // 这句话是我后加的

        t_scheduler = this;
    }

    Scheduler::~Scheduler()
    {
        assert(m_stop);
        if (GetThis() == this)
        {
            t_scheduler = nullptr;
        }
    }

    void Scheduler::run()
    {
        ::signal(SIGPIPE, SIG_IGN); // 这里为什么要忽略 sigpipe？

        setThis();

        Fiber::EnableFiber(); //在当前线程中启动协程

        // acid::set_hook_enable(true); // 这个函数暂时不知道有什么用

        Fiber::ptr cb_fiber;
        Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::wait, this)));

        Task task;

        while (true)
        {
            task.reset();
            bool tickle = false;
            //线程取出任务
            {
                MutexType::guard guard(m_mutex);
                auto it = m_tasks.begin();
                while (it != m_tasks.end())
                {
                    if (it->thread != -1 && GetThreadId() != it->thread)
                    {
                        ++it;
                        tickle = true;
                        continue;
                    }
                    assert(*it);
                    if (it->fiber && it->fiber->getState() == Fiber::EXEC)
                    {
                        ++it;
                        continue;
                    }
                    task = *it;
                    m_tasks.erase(it++);
                    break;
                }
                if (it != m_tasks.end())
                {
                    tickle = true;
                }
            }

            if (tickle)
            {
                notify();
            }

            if (task.fiber && (task.fiber->getState() != Fiber::TERM && task.fiber->getState() != Fiber::EXCEPT))
            {
                ++m_activeThreads;
                task.fiber->resume();
                --m_activeThreads;
                if (task.fiber->getState() == Fiber::READY)
                {
                    submit(task.fiber);
                }
                task.reset();
            }
            else if (task.cb)
            {
                if (cb_fiber)
                {
                    cb_fiber->reset(task.cb);
                }
                else
                {
                    cb_fiber.reset(new Fiber(task.cb));
                }
                task.reset();
                ++m_activeThreads;
                cb_fiber->resume();
                --m_activeThreads;
                if (cb_fiber->getState() == Fiber::READY)
                {
                    submit(cb_fiber);
                    cb_fiber.reset();
                }
                else if (cb_fiber->isTerminate())
                {
                    cb_fiber->reset(nullptr);
                }
                else
                {
                    cb_fiber = nullptr;
                }
            }
            else
            {

                if (idle_fiber->getState() == Fiber::TERM)
                {
                    break;
                }
                ++m_idleThreads;
                idle_fiber->resume();
                --m_idleThreads;
            }
        }
    }

    void Scheduler::start()
    {
        MutexType::guard guard(m_mutex);
        //调度器没有停止就直接返回
        if (m_stop == false)
        {
            return;
        }
        m_stop = false;

        assert(m_threads.empty());

        m_threads.reserve(m_threadCount);
        m_threadIds.reserve(m_threadCount);

        for (size_t i = 0; i < m_threadCount; ++i)
        {
            m_threads.emplace_back(std::thread([this]
                                               { this->run(); }));
            m_threadIds.push_back(i);
        }
    }

    void Scheduler::stop()
    {
        m_stop = true;
        for (size_t i = 0; i < m_threadCount; i++)
        {
            notify();
        }
        std::vector<std::thread> vec;

        vec.swap(m_threads);
        for (auto &t : vec)
        {
            t.join();
        }
    }

    Scheduler *Scheduler::GetThis()
    {
        return t_scheduler;
    }

    void Scheduler::setThis()
    {
        t_scheduler = this;
    }

    bool Scheduler::stopping()
    {
        MutexType::guard guard(m_mutex);
        return m_stop && m_tasks.empty() && m_activeThreads == 0;
    }

    void Scheduler::notify()
    {
        // 他这里只有一句log，是什么都不需要加吗
    }

    void Scheduler::wait()
    {

        std::cout << "idle";
        while (!stopping())
        {
            Fiber::YieldToHold(); //这句话之后注意一下
        }
        std::cout << "idle fiber exit";
    }

}
