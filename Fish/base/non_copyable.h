#pragma once


class NonCopyable
{
    public:
    NonCopyable() = default;
    ~NonCopyable() = default;


    NonCopyable(const NonCopyable& another) = delete;
    NonCopyable& operator=(const NonCopyable& another) = delete;



};

