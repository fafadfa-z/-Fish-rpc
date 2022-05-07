#include "net/buffer.h"
#include <cassert>

namespace Fish
{
    Buffer::Buffer(int size)
        :buf_(new char[size])
        ,size_(size)
        ,writeIndex_(0)
    {
    }

    Buffer::~Buffer()
    {
        delete buf_;
    }

    void Buffer::input(char *buf, size_t len)
    {
        input(std::string_view{buf, len});
    }

    void Buffer::input(const std::string_view view)
    {

        if ((int)view.size() > size_ - writeIndex_)
        {
            assert(false);
        }

        std::copy(view.data(), view.data() + view.size(), buf_ + writeIndex_);

        writeIndex_ += view.size();
    }

    const std::string_view Buffer::disBuf() const
    {
        return std::string_view(buf_,writeIndex_);
    }

    char*Buffer::disData()
    {
        return buf_;
    }
    const size_t Buffer::freeSize() const
    {
        auto size = size_ - writeIndex_ - 1;

        assert(size>=0);

        return static_cast<size_t>(size);
    }

    void Buffer::already(size_t len)
    {
        writeIndex_+=len;

        assert(writeIndex_<size_);
    }

    void Buffer::clear()
    {
        writeIndex_ = 0;
    }

}
