#pragma once

#include <memory>
#include <functional>
#include <ucontext.h>


namespace Fish
{

    class Fiber : public std::enable_shared_from_this<Fiber>
    {

    public:
        using ptr = std::shared_ptr<Fiber>;

        enum State
        {
            INIT,
            HOLD,
            EXEC,
            TERM,
            READY,
            EXCEPT
        };

        // 构造，用来构造普通协程
        Fiber(std::function<void()> cb, size_t stackSize = 0);
        ~Fiber();

        //重置协程执行函数，并重置状态
        void reset(std::function<void()> cb);
        //切换到当前协程
        void resume();
        //让出当前协程
        void yield();

        uint64_t id() const { return id_; }
        
        State getState() const { return state_; }

        bool isTerminate() const
        {
            return state_ == TERM || state_ == EXCEPT;
        }

        //在当前线程启用协程
        static void EnableFiber();

        //获取当前协程
        static Fiber::ptr GetThis();
        //获取当前协程Id
        static uint64_t GetFiberId();
        //设置当前协程
        static void SetThis(Fiber *f);

        //让出协程，并设置协程状态为Ready
        static void YieldToHold();
        //让出协程，并设置协程状态为Hold
        static void YieldToReady();
        //协程总数
        static uint64_t TotalFibers();

        static void MainFunc();

    private:
        Fiber();

    private:
        uint64_t id_ = 0;

        State state_ = INIT;

        uint32_t stackSize_ = 0;

        ucontext_t ctx_; //GCC 的协程库。 用来保存协程上下文

        void* stack_ = nullptr;  //协程的栈指针

        std::function<void()> cb_; //协程运行的函数

    };

}
