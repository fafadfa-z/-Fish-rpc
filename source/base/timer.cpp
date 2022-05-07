#include "base/timer.h"
#include <chrono>
#include <cassert>

namespace Fish
{
    using namespace std::chrono;

    uint64_t getCurrentMS() //获取当前的时间
    {
        auto timePoint = system_clock::now();

        auto result = duration_cast<milliseconds>(timePoint.time_since_epoch());

        return result.count();
    }

    uint64_t GetCurrentUS() //获取当前的时间
    {
        auto timePoint = system_clock::now();

        auto result = duration_cast<microseconds>(timePoint.time_since_epoch());

        return result.count();
    }

    bool Timer::Compare::operator()(const Timer::ptr &lhs, const Timer::ptr &rhs) const
    {
        // if (!lhs && !rhs)
        //     return false;

        // if (!lhs)
        //     return true;

        // if (!rhs)
        //     return false;

        // if (lhs->next_ < rhs->next_)
        //     return true;

        // if (lhs->next_ > rhs->next_)
        //     return false;

        // return lhs.get() < rhs.get();
    }

    Timer::Timer(uint64_t ms, std::function<void()> cb, bool priod, TimeManager *manager)
        : ms_(ms), next_(getCurrentMS() + ms), cb_(cb), period_(priod), manager_(manager)
    {
    }
    // Timer::Timer(uint64_t next) : next_(next)
    // {
    // }

    // void Timer::cancel()
    // {
    //     assert(manager_ != nullptr);
    //     assert(cb_);

    //     TimeManager::MutexType::guard guard(manager_->mut_);

    //     auto it = manager_->timers_.find(shared_from_this());

    //     assert(it != manager_->timers_.end());

    //     manager_->timers_.erase(it);
    // }

    // void Timer::refresh()
    // {
    //     assert(manager_ != nullptr);
    //     assert(cb_);

    //     TimeManager::MutexType::guard guard(manager_->mut_);

    //     auto it = manager_->timers_.find(shared_from_this());

    //     assert(it != manager_->timers_.end());

    //     manager_->timers_.erase(it);

    //     next_ = ms_ + getCurrentMS();

    //     manager_->timers_.insert(shared_from_this());
    // }
    void Timer::reset(uint64_t ms, bool from_now)
    {
    }

    Timer::ptr TimeManager::addTimer(uint64_t ms, std::function<void()> cb, bool priod)
    {
        Timer::ptr timer(new Timer(ms,cb,priod,this));
        MutexType::guard guard(mut_);



    }

    void addTimer(Timer::ptr& timer)
    {



    }

    Timer::ptr TimeManager::addConnditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> cond,bool priod)
    {


    }

    uint64_t TimeManager::getNextTimer()
    {




    }

    bool TimeManager::hasTimer()
    {
        MutexType::guard guard(mut_);

        return !timers_.empty();
    }


}
