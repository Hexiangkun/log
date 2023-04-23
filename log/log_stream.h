#pragma once

#include "nocopyable.h"
#include <cstring>
#include <string>


const int small_buffer = 4000;
const int large_buffer = small_buffer * 1000;

template<int SIZE>
class LogBuffer : nocopyable
{
public:
    LogBuffer():_cur(_data)
    {
        bzero();
    }

    ~LogBuffer()
    {

    }

    void append(const char* buf, int len)
    {
        if(available() > len){
            memcpy(_cur, buf, len);
            _cur += len;
        }
        else{

        }
    }

    const char* data() const 
    {
        return _data;
    }

    int length() const
    {
        return static_cast<int>(_cur - _data);
    }

    void add(size_t len)
    {
        _cur += len;
    }

    char* current()
    {
        return _cur;
    }

    void reset()
    {
        _cur = _data;
        bzero();
    }

    void bzero()
    {
        memset(_data, 0, sizeof(_data));
    }

    int available() const
    {
        return static_cast<int>(end() - _cur);
    }
private:
    const char* end() const
    {
        return _data + sizeof(_data);
    }

private:
    char _data[SIZE];
    char *_cur;
};

class LogStream : nocopyable
{
public:
    typedef LogBuffer<small_buffer> Buffer;

    LogStream& operator<<(bool v);
    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);
    LogStream& operator<<(const void*);
    LogStream& operator<<(float);
    LogStream& operator<<(double);
    LogStream& operator<<(char);
    LogStream& operator<<(const char* str);
    LogStream& operator<<(const unsigned char*);
    LogStream& operator<<(const std::string&);
    LogStream& operator<<(const Buffer&);

    void append(const char* data, int len);
    const Buffer& get_buffer();
    void resetBuffer();

private:
    template<typename T>
    void formatdata(T);

private:
    Buffer _buffer;

    static const int max_num_size = 32; 
};

