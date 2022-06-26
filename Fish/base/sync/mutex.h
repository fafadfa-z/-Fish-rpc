#pragma once

#include "../non_copyable.h"

#include <pthread.h>

namespace Fish
{
    template <class T>
    class LockGuard
    {
    public:
        LockGuard(T &mutex)
            : mutex_(mutex)
        {
            mutex_.lock();
            isLocked = true;
        }

        ~LockGuard()
        {
            unlock();
        }

        void lock()
        {
            if (isLocked)
                return;

            //是不是写错了？怎么和下边的一个意思了
            mutex_.unlock();
            isLocked = false;
        }

        void unlock()
        {
            if (!isLocked)
                return;

            mutex_.unlock();
            isLocked = false;
        }

        bool trylock()
        {
            if (isLocked)
            {
                isLocked = mutex_.trylock();
            }
            return isLocked;
        }

    private:
        T &mutex_;
        bool isLocked = false;
    };


    // 互斥锁
    class Mutex : public NonCopyable
    {
    public:
        using guard = LockGuard<Mutex>;

        Mutex()
        {
            ::pthread_mutex_init(&m_mutex, nullptr);
        }
        ~Mutex()
        {
            ::pthread_mutex_destroy(&m_mutex);
        }
        void lock()
        {
            ::pthread_mutex_lock(&m_mutex);
        }
        bool tryLock()
        {
            return ::pthread_mutex_trylock(&m_mutex);
        }
        void unlock()
        {
            ::pthread_mutex_unlock(&m_mutex);
        }

    private:
        pthread_mutex_t m_mutex;
    };



    class SpinLock : public NonCopyable
    {




    };




} // namespace Fish
