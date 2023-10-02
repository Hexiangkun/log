#pragma once

#include "nocopyable.h"
#include <cstdio>
#include <string>
#include <climits>
#include <mutex>
#include <memory>


class FileWriter : nocopyable
{
public:
    explicit FileWriter(std::string file_name);

    ~FileWriter();

    off_t get_written_bytes() const;

    void append(const char* line, const size_t len);

    void flush();

private:
    FILE* _file;
    char _buffer[64*1024];
    off_t written_bytes;
};



class LogFile : nocopyable
{
public:
    LogFile(off_t roll_size);
    ~LogFile();

    void append(const char* line, const size_t len);
    void flush();
    void rollFile();

private:
    void setBaseName();
    std::string getLogFileName();

private:
    char linkname[PATH_MAX];//软连接文件名
    char basename[PATH_MAX];//日志文件前缀
    off_t roll_size;    //日志文件大小
    int file_index;     //当前日志文件序号
    std::mutex _mutex;
    std::unique_ptr<FileWriter> _file;
};