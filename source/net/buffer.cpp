#include "net/buffer.h"
#include <cassert>

namespace Fish
{
    Buffer::Buffer(int size)
        : buf_(size), writeIndex_(0), readIndex_(0)
    {
    }

    Buffer::~Buffer()
    {
    }

    void Buffer::input(char *buf, size_t len)
    {
        input(std::string_view{buf, len});
    }

    void Buffer::input(const std::string_view view)
    {

        if (view.size() > buf_.size() - writeIndex_)
        {
            BufferType temp(writeIndex_ - readIndex_ + view.size());

            std::copy(buf_.begin() + readIndex_, buf_.begin() + writeIndex_, temp.begin());

            std::swap(buf_, temp);

            readIndex_ = 0;

            writeIndex_ = buf_.size();
        }

        std::copy(view.data(), view.data() + view.size(), &buf_[writeIndex_]);

        writeIndex_ += view.size();
    }

    const std::string_view Buffer::disRead() const
    {
        return std::string_view(&buf_[readIndex_], writeIndex_ - readIndex_);
    }

    const std::string_view Buffer::disWrite() const
    {
        auto size = buf_.size() - writeIndex_ - 1;
        assert(size >= 0);
        
        return std::string_view(&buf_[writeIndex_], size);
    }

    void Buffer::eraseData(size_t len)
    {
        assert(readIndex_ + (int)len <= writeIndex_);

        if (readIndex_ + (int)len == writeIndex_)
        {
            readIndex_ = writeIndex_ = 0;
        }
        else
        {
            readIndex_ += len;
        }
    }

    void Buffer::already(size_t len)
    {
        writeIndex_ += len;

        assert((size_t)writeIndex_ < buf_.size());
    }

    void Buffer::clear()
    {
        writeIndex_ = 0;
        readIndex_ = 0;
    }

}
