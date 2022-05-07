#pragma once

#include <vector>
#include <algorithm>
#include <string_view>
#include <base/non_copyable.h>
#include <utility>

namespace Fish
{
    class Buffer : public NonCopyable
    {
    public:
        using BufferType = char*;

        Buffer(int size);
        ~Buffer();

        //主动复制数据
        void input(char *buf, size_t len);
        void input(const std::string_view);

        char* disData();

        void already(size_t len);

        const std::string_view disBuf() const;

        const size_t freeSize() const;

        void clear();

    private:
        BufferType buf_;

        int size_;
        int writeIndex_;
    };
}
