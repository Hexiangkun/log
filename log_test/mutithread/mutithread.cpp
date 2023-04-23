#include "/home/hxk/C++Project/C++11webserver/code/log/log.h"
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>


#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

int64_t get_current_millis(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void thdo()
{
    for (int i = 0;i < 1e6; ++i)
    {
        LOG_INFO << "my number is number my number is my number is my number is my number is my number is my number is  ";
    }
}

int main(int argc, char** argv)
{
    SET_LOG_ASYNC(1);
    uint64_t start_ts = get_current_millis();

    std::vector<std::thread> tds;
    for (int i = 0; i < 4; ++i) {
        tds.push_back(std::thread(thdo));
    }

    for(size_t i = 0; i < 4; ++i) {
        tds[i].join();
    }
    uint64_t end_ts = get_current_millis();
    printf("time use %lums\n", end_ts - start_ts);
}
