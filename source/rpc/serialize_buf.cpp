#include "rpc/serialize_buf.h"
#include <string.h>
#include <iostream>

namespace Fish
{
    void SeriBuf::input(const void *buf, unsigned long len) //将外面的数据输入到数组里。
    {
        const char *buf_end = static_cast<const char *>(buf) + len;

        buf_.insert(buf_.end(), static_cast<const char *>(buf), buf_end);
    }

    void SeriBuf::output(void *buf, unsigned long len) //将数据输出到外面
    {
        [[unlikely]] if (len > buf_.size())
        {
            len = buf_.size();
            std::cout<<"Rpc: 索取了超过容量的数据"<<std::endl;
        }

        std::copy(buf_.begin(),buf_.begin()+len, static_cast<char *>(buf));

        buf_.erase(buf_.begin(),buf_.begin()+len);
    }

    void SeriBuf::input_int8(int8_t value)
    {
        SeriBuf::input(reinterpret_cast<char *>(&value), sizeof(value));
    }
    void SeriBuf::input_int16(int16_t value)
    {
        SeriBuf::input(&value, sizeof(value));
    }
    void SeriBuf::input_int32(int32_t value)
    {
        SeriBuf::input(&value, sizeof(value));
    }
    void SeriBuf::input_int64(int64_t value)
    {
        SeriBuf::input(&value, sizeof(value));
    }
    void SeriBuf::input_uint8(uint8_t value)
    {
        SeriBuf::input(&value, sizeof(value));
    }
    void SeriBuf::input_uint16(uint16_t value)
    {
        SeriBuf::input(&value, sizeof(value));
    }
    void SeriBuf::input_uint32(uint32_t value)
    {
        SeriBuf::input(&value, sizeof(value));
    }
    void SeriBuf::input_uint64(uint64_t value)
    {
        SeriBuf::input(&value, sizeof(value));
    }
    void SeriBuf::input_float(float value)
    {
        SeriBuf::input(&value, sizeof(value));
    }
    void SeriBuf::input_double(double value)
    {
        SeriBuf::input(&value, sizeof(value));
    }

    void SeriBuf::input_str(char *value)
    {
        auto size = ::strlen(value);

        SeriBuf::input(&size, sizeof(size));
        SeriBuf::input(value, size);
    }
    void SeriBuf::input_string(const std::string &value)
    {
        auto size = value.size();
        SeriBuf::input(&size, sizeof(size_t));
        SeriBuf::input(value.c_str(), value.size());
    }
    void SeriBuf::input_string_view(std::string_view value)
    {
        auto size = value.size();
        SeriBuf::input(&size, sizeof(size_t));
        SeriBuf::input(value.data(), value.size());
    }

    int8_t SeriBuf::output_int8()
    {
        int8_t result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }
    int16_t SeriBuf::output_int16()
    {
        int16_t result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }
    int32_t SeriBuf::output_int32()
    {
        int32_t result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }
    int64_t SeriBuf::output_int64()
    {
        int64_t result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }
    uint8_t SeriBuf::output_uint8()
    {
        uint8_t result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }
    uint16_t SeriBuf::output_uint16()
    {
        uint16_t result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }
    uint32_t SeriBuf::output_uint32()
    {
        uint32_t result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }

    uint64_t SeriBuf::output_uint64()
    {
        uint64_t result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }

    float SeriBuf::output_float()
    {
        float result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }

    double SeriBuf::output_double()
    {
        double result;
        SeriBuf::output(&result, sizeof(result));
        return result;
    }
    std::string SeriBuf::output_string()
    {
        size_t size;
        
        SeriBuf::output(&size, sizeof(size));

        std::string result;
        SeriBuf::output(&result[0], size);
        return result;
    }

}
