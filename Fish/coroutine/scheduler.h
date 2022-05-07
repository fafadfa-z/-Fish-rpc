#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include <list>
#include "base/sync/mutex.h"

#include "fiber.h"

namespace Fish
{

    class Scheduler
    {
    public:
        using ptr = std::shared_ptr<Scheduler>;
        using MutexType = Mutex;

        Scheduler(size_t threads = 4, const std::string &name = "");

        virtual ~Scheduler();

        const std::string &name() const { return m_name; }

        void start(); // 开始调度

        void stop(); // 停止调度

        template <class FiberOrCb>
        [[maybe_unused]] Scheduler *submit(FiberOrCb &&fc, int thread = -1)
        {
            bool need_notify = false;

            {
                MutexType::guard guard(m_mutex);
                need_notify = submitNoLock(std::forward<FiberOrCb>(fc), thread);
            }

            if (need_notify)
                notify();

            return this;
        }

        template <class InputIterator>
        void submit(InputIterator begin, InputIterator end)
        {
            bool need_notify = false;
            {
                MutexType::guard guard(m_mutex);
                while (begin != end)
                {
                    need_notify = submitNoLock(std::move(*begin), -1) || need_notify;
                    ++begin;
                }
            }
            if (need_notify)
            {
                notify();
            }
        }

        template <class FiberOrCb>
        [[maybe_unused]] Scheduler *operator+(FiberOrCb &&fc)
        {
            return submit(std::move(fc));
        }


        static Scheduler *GetThis();

    private:
        template <class FiberOrCb>
        bool submitNoLock(FiberOrCb &&fc, int thread)
        {
            bool need_notify = m_tasks.empty();

            Task task(std::forward<FiberOrCb>(fc), thread);

            if (task) // 如果有回调函数，则加入任务队列
            {
                m_tasks.push_back(task);
            }
            return need_notify;
        }

    protected:
        virtual void notify();  //同时有任务需要调度

        void run();

        virtual bool stopping(); //返回是否可以停止


        virtual void wait();  //没有任务可执行的时候，执行的函数


        void setThis();


        bool hasIdleThreads()       //返回是否有空闲的线程
        { return m_idleThreads > 0; }

    private:
        struct Task
        {
            Fiber::ptr fiber;

            std::function<void()> cb;

            int thread;

            Task(Fiber::ptr &f, int t = -1)
            {
                fiber = f;
                thread = t;
            }
            Task(Fiber::ptr &&f, int t = -1)
            {
                fiber = std::move(f);
                thread = t;
                f = nullptr;
            }
            Task(std::function<void()> &f, int t = -1)
            {
                cb = f;
                thread = t;
            }
            Task(std::function<void()> &&f, int t = -1)
            {
                cb = std::move(f);
                thread = t;
                f = nullptr;
            }
            Task()
            {
                thread = -1;
            }

            void reset()
            {
                thread = -1;
                fiber = nullptr;
                cb = nullptr;
            }
            operator bool() //此类和携带的fiber 类里面只要有一个有回调函数就行
            {
                return fiber || cb;
            }

            /* data */
        };

    private:
        MutexType m_mutex;

        //调度器线程池
        std::vector<std::thread> m_threads;
        //调度器任务
        std::list<Task> m_tasks;

        std::string m_name;

    protected:
        std::vector<int> m_threadIds;

        //线程数量
        size_t m_threadCount = 0;
        //活跃线程数
        std::atomic<size_t> m_activeThreads = 0;
        //空闲线程数
        std::atomic<size_t> m_idleThreads = 0;
        //调度器是否停止
        bool m_stop = true;
    };

}
