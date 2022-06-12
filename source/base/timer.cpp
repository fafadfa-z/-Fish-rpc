#include "base/timer.h"
#include <thread>
#include <chrono>

#include "base/log/logger.h"

namespace Fish
{
    using namespace std::chrono;

    void delay_ms(int ms)
    {
        auto begin = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        begin += ms;

        while (begin > duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count())
        {
            std::this_thread::yield();
        }
    }

    TimerTask::TimerTask(int time, bool repeatable, int id)
        : repeatable_(repeatable),
          id_(id)
    {
        auto nowTime = system_clock::now().time_since_epoch();

        insertTime_ = duration_cast<milliseconds>(nowTime).count();

        timePoint_ = insertTime_ + time;
    }

    void NeedTime::work()
    {
        auto now = system_clock::now().time_since_epoch();
        auto timeDur = duration_cast<milliseconds>(now).count();

        task_(timeDur - insertTime_);

        insertTime_ = timeDur;
    }

    Timer *Timer::init(int time)
    {

        [[unlikely]]if (entity_ == nullptr)
        {
            LOG_INFO<<"Timer: create timer.."<<Fish::end;

            entity_ = new Timer(time);
        }
        else
        {
            LOG_INFO<<"Timer: return timer.."<<Fish::end;
        }
        return entity_;
    }
    int64_t Timer::getNow_Milli()
    {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    Timer::Timer(int timeInterval)
        : timeNum_(1000 / timeInterval),
          timeWheel_(timeNum_),
          mutexVec(timeNum_),
          wheelIndex_(0),
          working_(false)
    {
    }

    Timer::~Timer()
    {
        for (auto &list : timeWheel_)
        {
            for (auto &timer : list)
            {
                delete timer;
            }
        }
    }

    void Timer::addOnceTask(std::function<void()> task, int time) // time 单位：ms
    {
        int index = time;

        index /= 1000 / timeNum_; //计算几个槽位之后进行

        index %= timeNum_; //去掉整圈的克度，需要向下移动多少个槽位

        index = (index + wheelIndex_) / timeNum_; //计算需要放入的槽位

        auto *timer = new NotNeedTime(time);

        timer->setTask(task);

        std::lock_guard guard(mutexVec[index]);

        timeWheel_[index].push_back(timer);
    }

    void Timer::addOnceTask(std::function<void(int)> task, int time)
    {
        int index = time;

        index /= 1000 / timeNum_;

        index %= timeNum_;

        index = (index + wheelIndex_) / timeNum_;

        auto *timer = new NeedTime(time);

        timer->setTask(task);

        std::lock_guard guard(mutexVec[index]);

        timeWheel_[index].push_back(timer);
    }

    bool Timer::addPriodTask(std::function<void()> task, int time, int id)
    {
        NotNeedTime *timer;
        {
            std::lock_guard guard(priodTimerGuarder_);

            auto iter = priodTimerMap_.find(id);

            if (iter != priodTimerMap_.end())
                return false;

            timer = new NotNeedTime(time, true, id);

            priodTimerMap_.insert(std::move(std::pair{id, timer}));
        }

        int index = time;

        index /= 1000 / timeNum_;

        index %= timeNum_;

        index = (index + wheelIndex_) / timeNum_;

        timer->setTask(task);

        std::lock_guard guard(mutexVec[index]);

        timeWheel_[index].push_back(timer);

        return true;
    }

    bool Timer::addPriodTask(std::function<void(int)> task, int time, int id)
    {
        NeedTime *timer;

        {
            std::lock_guard guard(priodTimerGuarder_);

            auto iter = priodTimerMap_.find(id);

            if (iter != priodTimerMap_.end())
                return false;

            timer = new NeedTime(time, true, id);

            priodTimerMap_.insert(std::move(std::pair{id, timer}));
        }

        int index = time;

        index /= 1000 / timeNum_;

        index %= timeNum_;

        index = (index + wheelIndex_) / timeNum_;

        timer->setTask(task);

        std::lock_guard guard(mutexVec[index]);

        timeWheel_[index].push_back(timer);

        return true;
    }

    bool Timer::removePriodTask(int id)
    {
        std::lock_guard guard(priodTimerGuarder_);

        auto iter = priodTimerMap_.find(id);

        if (iter == priodTimerMap_.end())
            return false;

        iter->second->canDelete_ = true;

        priodTimerMap_.erase(iter);

        return true;
    }

    void Timer::start()
    {
        if (working_)
            return;

        working_ = true;

        long timeBegin = 0;
        long timeEnd = 0;

        while (working_)
        {
            delay_ms(1000 / timeNum_ - (timeEnd - timeBegin));

            timeBegin = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            std::vector<TimerTask *> timeOutVec;
            timeOutVec.reserve(5);

            mutexVec[wheelIndex_].lock();

            auto &wheelList = timeWheel_[wheelIndex_];

            for (auto iter = wheelList.begin(); iter != wheelList.end(); iter++)
            {
                auto timer = *iter;
                if(timer->canDelete_)
                {
                    iter = wheelList.erase(iter); //小心这里迭代器失效的问题
                }
                else if (timer->timePoint() <= timeBegin)
                {
                    timeOutVec.push_back(timer);

                    if (!timer->repeatable())
                        iter = wheelList.erase(iter); //小心这里迭代器失效的问题
                }
            }
            mutexVec[wheelIndex_].unlock();

            for (auto timer : timeOutVec) //执行超时的定时器。
            {
                timer->work();

                if (!timer->repeatable())
                    delete timer;
            }

            timeOutVec.clear();

            wheelIndex_++;

            if (wheelIndex_ == timeNum_)
                wheelIndex_ = 0;

            timeEnd = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        }
    }
}
