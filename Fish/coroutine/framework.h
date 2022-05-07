#pragma once

#include <coroutine>
#include <iostream>
#include <memory>
#include <string_view>

namespace Fish
{
    enum task_option
    {
        READ,
        WRITE,
    };

    class Uring;
    class Channel;

    struct UringRequest
    {
        int event_type;

        int client_socket;

        Channel* channel;
    };

    class Task
    {
    public:
        using ptr = std::shared_ptr<Task>;

        struct promise_type
        {
            using Handle = std::coroutine_handle<promise_type>;

            Task get_return_object()
            {
                return Task{Handle::from_promise(*this)};
            }
            std::suspend_never initial_suspend() noexcept
            {
                return {};
            }
            std::suspend_never final_suspend() noexcept
            {
                return {};
            }
            void return_void() noexcept
            {
            }
            void unhandled_exception() noexcept
            {
                std::cout << "exception in coroutine..." << std::endl;
            }

            // Channel* channel_;
        };

    public:
        explicit Task(promise_type::Handle handler) : handler(handler)
        {
        }
        Task() = default;

        Task(const Task &another)
            : handler(another.handler)
        {
        }

        void destroy() { handler.destroy(); }

    public:
        promise_type::Handle handler;
    };

    class ReadWaitor
    {
    public:
        ReadWaitor(Channel &);

        bool await_ready() { return false; }

        void await_suspend(std::coroutine_handle<Task::promise_type>);

        std::string_view await_resume();

    private:
        Channel &channel_;
        Task::promise_type *promise_ = nullptr;
    };

}
