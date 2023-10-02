#include "../log/log.h"

#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

std::atomic<long> count(0);

void Thread_Proc(int index)
{
    while(true)
    {
        ++count;
        LOG_INFO << "thread index:" <<index<<"; count:"<<count<<"; 012";
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


int main()
{
    SET_LOG_ASYNC(1);

    std::vector<std::thread> tds;
    for (int i = 0; i < 4; ++i) {
        tds.push_back(std::thread(Thread_Proc, i));
    }

    for(size_t i = 0; i < 4; ++i) {
        tds[i].detach();
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }


    return 0;
}


