#include "Timestamp.h"

Timestamp::Timestamp(/* args */)
{
}

Timestamp::~Timestamp()
{
}

int64_t Timestamp::now()
{
    struct  timeval tv;
    gettimeofday(&tv, nullptr);

    return tv.tv_sec*micro_sec_persec + tv.tv_usec;
}