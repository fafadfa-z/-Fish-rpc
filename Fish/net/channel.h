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

        void setCallBack(std::function<void(Channel::ptr)> cb) 
        { cb_ = cb; }

        void setCloseCallBack(std::function<void(int)> cb) 
        { closeCb_ = cb; }


        auto disPtr(){return buf_.disData();}

        auto disView(){return buf_.disBuf();}

        auto eraseFront(size_t len) {buf_.eraseData(len);}
        auto alreadyRead(size_t len ){buf_.already(len);} //由外部直接输入的数据

        void clearBuf(){ buf_.clear();}  //清空buf

        void close(); //通道关闭

        void send(std::string_view);  //发送数据(提交发送申请)


        void reduseSend();  //有发送请求完成后调用


        void operator()()
        {
            assert(cb_);
            cb_(shared_from_this());
        }

        auto co_read()   //由协程调用，返回awaitor
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

        std::function<void(Channel::ptr)> cb_;
        std::function<void(int)> closeCb_;

        int fd_;
    };
}
