#pragma once

#include <vector>
#include <atomic>

class Buffer
{

public:
    Buffer(int init_buf_size = 1024);
    ~Buffer();
private:
    std::vector<char> buffer_;
    std::atomic<std::size_t> readPos_;
    std::atomic<std::size_t> writePos_;
};


Buffer::~Buffer()
{
}
