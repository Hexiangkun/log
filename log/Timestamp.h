#pragma once
#include <cstdint>
#include <sys/time.h>

class Timestamp
{
private:
    /* data */
public:
    Timestamp(/* args */);
    ~Timestamp();

    static int64_t now();
    static const int micro_sec_persec = 1000*1000;
};


