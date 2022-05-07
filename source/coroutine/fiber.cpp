#include "coroutine/fiber.h"
#include <cassert>
#include <atomic>
#include <exception>
#include <iostream>

namespace Fish
{
    static std::atomic<uint64_t> fiber_count(0);
    static std::atomic<uint64_t> fiber_id(0);

    //当前协程指针
    static thread_local Fiber *current_fiber_ptr = nullptr;

    //当前线程的主协程指针
    static thread_local Fiber::ptr main_fiber = nullptr;

    Fiber::Fiber() // 主协程构造函数
    {
        state_ = EXEC;
        SetThis(this);

        auto ret = ::getcontext(&ctx_);

        assert(!ret);

        fiber_count++;
    }

    Fiber::Fiber(std::function<void()> cb, size_t stackSize)
        : id_(++fiber_id),
          cb_(cb)
    {
        assert(main_fiber != nullptr);

        fiber_count++;

        stackSize_ = stackSize ? stackSize : 128 * 1024;

        stack_ = ::malloc(stackSize_);

        auto ret = ::getcontext(&ctx_);

        assert(!ret);

        ctx_.uc_link = nullptr;
        ctx_.uc_stack.ss_size = stackSize_;
        ctx_.uc_stack.ss_sp = stack_;

        makecontext(&ctx_, MainFunc, 0);
    }

    Fiber::~Fiber()
    {
        --fiber_count;
        if (stack_)
        {
            assert(state_ == INIT || state_ == TERM || state_ == EXCEPT);

            ::free(stack_);
        }
        else
        {
            assert(state_ == EXEC);
            assert(!cb_);
            if (current_fiber_ptr == this)
            {
                SetThis(nullptr);
            }
            assert(false); // 先加一个
        }
    }

    void Fiber::MainFunc()
    {
        Fiber::ptr cur = GetThis();

        assert(cur);

        try
        {
            cur->cb_();
            cur->cb_ = nullptr;
            cur->state_ = TERM;
        }
        catch (std::exception &ex)
        {
            cur->state_ = EXCEPT;
            std::cout << "Fiber Except: " << ex.what();
        }
        catch (...)
        {
            cur->state_ = EXCEPT;
            std::cout << "Fiber Except: -";
        }
        auto ptr = cur.get();
        cur = nullptr;
        ptr->yield();
    }

    void Fiber::reset(std::function<void()> cb)
    {
        assert(stack_);
        assert(state_ == INIT || state_ == TERM || state_ == EXCEPT);

        cb_ = cb;

        auto ret = ::getcontext(&ctx_);

        assert(ret == 0);

        ctx_.uc_link = nullptr;
        ctx_.uc_stack.ss_size = stackSize_;
        ctx_.uc_stack.ss_sp = stack_;

        ::makecontext(&ctx_, MainFunc, 0);

        state_ = INIT;
    }

    void Fiber::resume() //这个函数应该是由主协程调用的。
    {
        SetThis(this);

        assert(state_ != EXEC);

        state_ = EXEC;

        auto ret = swapcontext(&main_fiber->ctx_, &ctx_);

        assert(ret == 0);
    }

    void Fiber::yield() //这玩意应该是切换到主协程
    {
        SetThis(main_fiber.get());
        auto ret = ::swapcontext(&ctx_, &main_fiber->ctx_);

        assert(ret == 0);
    }

    void Fiber::SetThis(Fiber *f)
    {
        current_fiber_ptr = f;
    }

    Fiber::ptr Fiber::GetThis()
    {
        return current_fiber_ptr->shared_from_this();
    }

    void Fiber::YieldToHold()
    {
        Fiber::ptr cur = GetThis();
        cur->state_ = HOLD;
        cur->yield();
    }

    void Fiber::YieldToReady()
    {
        Fiber::ptr cur = GetThis();
        cur->state_ = READY;
        cur->yield();
    }

    uint64_t Fiber::TotalFibers()
    {
        return fiber_count;
    }

    uint64_t Fiber::GetFiberId()
    {
        if (current_fiber_ptr)
        {
            return current_fiber_ptr->id();
        }
        return 0;
    }

    void Fiber::EnableFiber()
    {
        if (!current_fiber_ptr)
        {
            Fiber::ptr main_f(new Fiber);
            assert(current_fiber_ptr == main_f.get());
            main_fiber = main_f;
        }
    }

}
