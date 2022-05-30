#include "coroutine/framework.h"

#include "net/uring_io.h"
#include "net/channel.h"

#include <iostream>

#include <cassert>

namespace Fish
{
    ReadWaitor::ReadWaitor(Channel &channel)
        :channel_(channel)
    {

    }

    void ReadWaitor::await_suspend(std::coroutine_handle<Task::promise_type> handle)
    {
       channel_.uring_->addReadRequest(channel_.fd());
    }

    std::string_view ReadWaitor::await_resume()
    {
        auto view = channel_.disView();

        // std::cout<<"await_resume()"<<std::endl;  //输出一下调试信息

        return {view.data(),view.size()};
    }


}
