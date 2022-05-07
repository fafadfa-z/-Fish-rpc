#pragma once 
#include <utility>
#include <string_view>

namespace Fish
{
    class Consumer
    {
        public:
        virtual ~Consumer()
        {

        }
        
        virtual bool judge(std::string_view) = 0;
        
        virtual void read(size_t) = 0;

        virtual void clear() = 0;
    };






    
} // namespace Fish













