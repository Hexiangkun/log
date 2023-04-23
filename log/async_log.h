#pragma once

#include "log_stream.h"
#include "nocopyable.h"

#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>
#include <atomic>

class AsyncLog : nocopyable
{
    typedef LogBuffer<large_buffer> Buffer; //日志缓冲区
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;  //日志缓冲区数组
    typedef BufferVector::value_type BufferPtr; //指向Buffer对象的独占智能指针

public:
    AsyncLog(int flush_interval=500, int roll_size=20*1024*1024);
    ~AsyncLog();

    void append(const char* buf, int len);

    void stop();

private:
    void writeThread();
private:
    const int flush_interval;//刷新时间间隔
    const int roll_size;    //日志文件大小阈值
    std::atomic<bool> running;//表示线程是否正在运行
    std::thread thread; //写日志线程
    std::mutex mtx; //保护共享资源
    std::condition_variable cond;   //线程之间的同步和通信
    BufferPtr cur_buffer;   //指向当前缓冲区的独占指针
    BufferPtr next_buffer;  //指向下一个缓冲区的独占指针
    BufferVector buffers;   //存储指向缓冲区的独占指针的动态数组
};