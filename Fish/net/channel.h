#pragma once

#include "buffer.h"
#include "base/non_copyable.h"
#include "coroutine/framework.h"
#include "net/uring_io.h"
#include <functional>
#include <cassert>
#include <memory>
#include <atomic>



namespace Fish
{
    class Channel : public NonCopyable, public std::enable_shared_from_this<Channel>
    {
    public:

        using ptr = std::shared_ptr<Channel>;

        Channel(int, Uring*,int = 1024);

        int fd() const { return fd_; }

        void setCallBack(std::function<bool()> cb) { cb_ = cb; }

        auto disPtr(){return buf_.disData();}
        auto disView(){return buf_.disBuf();}
        auto alreadyRead(size_t len ){buf_.already(len);}
        void clearBuf(){ buf_.clear();}

        void close();

        void send(std::string_view);
        void reduseSend(); 


        bool operator()()
        {
            assert(cb_);
            return cb_();
        }

        auto co_read()
        {
            return ReadWaitor(*this);
        }

        size_t freeSize() const {return buf_.freeSize();}

    public:
        Task task_;
        Buffer buf_;
        Uring* uring_;
    
    private:

        std::atomic<int> sendFlag_ = 0; // 标志当前是否有send的数据
        bool closeFlag_ = false;     //延时删除的标记位 

        std::function<bool()> cb_;

        int fd_;
    };
}
