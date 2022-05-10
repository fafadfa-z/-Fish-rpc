#pragma once

#include <deque>
#include <string>

#include <string_view>
#include <stdexcept>
#include <cassert>

namespace Fish::rpc
{
    class SeriBuf //序列化之后的buffer
    {
    public:
        SeriBuf() = default;

        void input(const void *buf, unsigned long len); //将外面的数据输入到数组里。

        void output(void* buf, unsigned long len); //将数据输出到外面

        void input_int8(int8_t);
        void input_int16(int16_t);
        void input_int32(int32_t);
        void input_int64(int64_t);

        void input_uint8(uint8_t);
        void input_uint16(uint16_t);
        void input_uint32(uint32_t);
        void input_uint64(uint64_t);

        void input_float(float);
        void input_double(double);

        void input_str(char *);
        void input_string(const std::string &);
        void input_string_view(std::string_view);

        int8_t output_int8();
        int16_t output_int16();
        int32_t output_int32();
        int64_t output_int64();
        uint8_t output_uint8();
        uint16_t output_uint16();
        uint32_t output_uint32();
        uint64_t output_uint64();
        float output_float();
        double output_double();
        std::string output_string();

    private:
        std::deque<char> buf_;
    };

}
