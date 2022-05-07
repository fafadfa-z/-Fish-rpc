#pragma once

#include "sync/mutex.h"

#include <memory>
#include <functional>
#include <set>

namespace Fish
{
    class TimeManager;

    class Timer : public std::enable_shared_from_this<Timer>
    {
        friend class TimeManager;
    public:
        using ptr = std::shared_ptr<Timer>;

        void cancel();
        void refresh();
        void reset(uint64_t ms, bool from_now);

        struct Compare
        {
            bool operator()(const Timer::ptr &, const Timer::ptr &) const;
        };

    private:
        uint64_t ms_ = 0;
        uint64_t next_ = 0;

        std::function<void()> cb_;

        bool period_ = false;

        TimeManager *manager_ = nullptr;

    private:
        Timer(uint64_t ms, std::function<void()> cb, bool priod, TimeManager *manager);
        Timer(uint64_t next);
    };

    class TimeManager
    {
        friend class Timer;

    public:
        using MutexType = Mutex;

        TimeManager() = default;
        ~TimeManager() = default;

        Timer::ptr addTimer(uint64_t ms, std::function<void()>cb,bool priod = false);

        Timer::ptr addConnditionTimer(uint64_t ms, std::function<void()>cb,std::weak_ptr<void>cond,bool priod = false);

        uint64_t getNextTimer();

        bool hasTimer();


    private:

        void addTimer(Timer::ptr& timer);


    private:
        MutexType mut_;
        std::set<Timer::ptr, Timer::Compare>timers_;
    };

} // namespace Fish
