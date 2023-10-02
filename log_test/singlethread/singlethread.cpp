#include "/home/hxk/git_project/log/log/log.h"
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

int64_t get_current_millis(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


int main(int argc, char** argv)
{
    SET_LOG_ASYNC(1);
    auto batch = 1e6;
    long g_total = 0;
    uint64_t start_ts = get_current_millis();
    for (int i = 0;i < 1e6; ++i)
    {
        g_total += 161;
        LOG_INFO << "my number is number my number is my number is my number is my number is my number is my number is  ";
    }
    
    uint64_t end_ts = get_current_millis();
    auto seconds = (end_ts - start_ts) * 1.0 / 1000;
    printf("%f seconds, %ld bytes, %.2f msg/s, %.2f MiB/s\n",
         seconds, g_total, batch / seconds, g_total / seconds / 1024 / 1024);

}