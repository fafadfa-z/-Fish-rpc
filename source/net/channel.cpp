#include "net/channel.h"

namespace Fish
{
    Channel::Channel(int fd, Uring* uring,int bufSize)
        : buf_(bufSize),uring_(uring), fd_(fd)
    {
    }

    void Channel::close()
    {
        std::cout << "Channel::close()" << std::endl;
        
        if(sendFlag_>0)
        {
            closeFlag_ = true;
            return;
        }

        if(closeCb_) closeCb_(fd_);

        task_.handler.destroy();   //协程断开

        uring_->removeFd(shared_from_this());
    }


    void Channel::send(std::string_view view)
    {
        if(closeFlag_) return;

        assert(uring_ != nullptr);

        uring_->addWriteRequest(fd_,view.data(),view.size());
        
        sendFlag_++;
    }

    void Channel::reduseSend()
    {
        assert(sendFlag_>0);

        sendFlag_--;

        if(sendFlag_ ==0 and closeFlag_)
            Channel::close();
    }
}
