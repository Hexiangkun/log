#include "log_stream.h"
#include <algorithm>


const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
const char digitsHex[] = "0123456789ABCDEF";

//将数值类型的参数转化为字符串数组，并返回该字符串数组长度
template<class T>
size_t convert(char buf[], T value)
{
    T i = value;
    char *p = buf;

    do{
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    }while(i != 0);
    if(value < 0){
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}

//用于将指针类型的值转换为16进制字符串
size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char *p =buf;

    do{
        int lsd = static_cast<int>(i%16);
        i /= 16;
        *p++ = digitsHex[lsd];
    }while(i != 0);

    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}

//格式化整型数据，将其添加进内部缓冲区
template<class T>
void LogStream::formatdata(T v)
{
    // if(_buffer.available() >= max_num_size){
    //     size_t len = convert(_buffer.current(), v);
    //     _buffer.add(len);
    // }

    std::string res = std::to_string(v);
    _buffer.append(res.c_str(),res.size());
}

LogStream& LogStream::operator<<(bool v)
{
    if(v){
        _buffer.append("true", 4);
    }
    else{
        _buffer.append("false", 5);
    }
    return *this;
}

LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v)
{
    formatdata(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    formatdata(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    formatdata(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    formatdata(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v)
{
    formatdata(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    formatdata(v);
    return *this;
}

LogStream& LogStream::operator<<(float v)
{
    //*this << static_cast<double>(v);
    formatdata(v);
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    // if(_buffer.available() > max_num_size){
    //     int len = snprintf(_buffer.current(), max_num_size, "%.12g", v);
    //     _buffer.add(len);
    // }
    formatdata(v);
    return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    if(_buffer.available() > max_num_size){
        char* buf = _buffer.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf + 2, v);
        _buffer.add(len + 2);
    }
    return *this;
}


LogStream& LogStream::operator<<(char v)
{
    _buffer.append(&v, 1);
    return *this;
}


LogStream& LogStream::operator<<(const char* str)
{
    if(str){
        _buffer.append(str, static_cast<int>(strlen(str)));
    }
    else{
        _buffer.append("(null)", 6);
    }
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char* str)
{
    *this << reinterpret_cast<const char*>(str);
    return *this;
}

LogStream& LogStream::operator<<(const std::string& str)
{
    _buffer.append(str.c_str(), static_cast<int>(str.size()));
    return *this;
}

LogStream& LogStream::operator<<(const Buffer& v)
{
    _buffer.append(v.data(), v.length());
    return *this;
}

void LogStream::append(const char* data, int len)
{
    _buffer.append(data, len);
}

const LogStream::Buffer& LogStream::get_buffer()
{
    return _buffer;
}


void LogStream::resetBuffer()
{
    _buffer.reset();
}


