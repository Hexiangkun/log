#include "async_log.h"
#include "log_file.h"

#include <chrono>
#include <cassert>

AsyncLog::AsyncLog(int flush_interval, int roll_size):flush_interval(flush_interval), roll_size(roll_size),
                                                    running(true),thread(std::bind(&AsyncLog::writeThread, this), "AsyncLog-inThread"),
                                                    cur_buffer(new Buffer), next_buffer(new Buffer),buffers()
{
    cur_buffer->bzero();
    next_buffer->bzero();
    buffers.reserve(8);
}

AsyncLog::~AsyncLog()
{
    if(running){
        stop();
    }
}

void AsyncLog::stop()
{
    running = false;
    thread.join();//主线程阻塞，等待被调线程终止，主线程回调被调线程资源，并继续运行
}

void AsyncLog::append(const char* buf, int len)
{
    std::unique_lock<std::mutex> lock(mtx);

    if(cur_buffer->available() > len){
        cur_buffer->append(buf, len);
    }
    else{
        buffers.push_back(std::move(cur_buffer));
        if(next_buffer){
            cur_buffer = std::move(next_buffer);
        }
        else{
            cur_buffer.reset(new Buffer);
            next_buffer.reset(new Buffer);
        }
        cur_buffer->append(buf, len);
        cond.notify_one();
    }
}

void AsyncLog::writeThread()
{
    BufferPtr new_buffer1(new Buffer);
    BufferPtr new_buffer2(new Buffer);
    new_buffer1->bzero();
    new_buffer2->bzero();

    BufferVector buffers_to_write;  //用于存储待写入磁盘的缓冲区
    buffers_to_write.reserve(8);

    LogFile ouput(roll_size);   //将缓冲区数据写入磁盘文件的对象

    while(running){
        {
            //获取互斥锁，并等待flush_interval时间后唤醒
            std::unique_lock<std::mutex> lock(mtx);
            cond.wait_for(lock, std::chrono::milliseconds(flush_interval));

            buffers.push_back(std::move(cur_buffer));//将当前缓冲区移动到buffers中
            cur_buffer = std::move(new_buffer1);
            buffers_to_write.swap(buffers);
            if(!next_buffer){
                next_buffer=std::move(new_buffer2);
            }
        }

        assert(!buffers_to_write.empty());

        //如果缓冲区大小大于16，打印错误信息，并删除其中多余缓冲区（保留前两个）
        if(buffers_to_write.size() > 16){
            char buf[256];
            snprintf(buf, sizeof(buf), "Dropped log messages %zd larger buffers\n", buffers_to_write.size()-8);
            fputs(buf, stderr);
            buffers_to_write.erase(buffers_to_write.begin()+8, buffers_to_write.end());
        }

        //遍历缓冲区数组，将数据追加到写入磁盘对象中去
        for(auto &buffer : buffers_to_write){
            ouput.append(buffer->data(), static_cast<size_t>(buffer->length()));
        }

        //如果
        if(buffers_to_write.size() > 2){
            buffers_to_write.resize(2);
        }

        if(!new_buffer1){
            assert(!buffers_to_write.empty());
            new_buffer1 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            //new_buffer1.reset();
        }

        if(!new_buffer2){
            assert(!buffers_to_write.empty());
            new_buffer2 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            //new_buffer2.reset();
        }
        buffers_to_write.clear();
        BufferVector().swap(buffers_to_write);
        buffers_to_write.reserve(8);
        ouput.flush();
    }
    ouput.flush();
}