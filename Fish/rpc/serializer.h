#pragma once

#include <memory>
#include "net/io_consumer.h"

namespace Fish::rpc
{

    class Serializer : public Consumer
    {
    public:
        using ptr = std::shared_ptr<Serializer>;

        bool judge(std::string_view) override;

        void read(size_t) override;

        void clear() override;
    };

}
