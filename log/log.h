#pragma once
#include <cstring>
#include <functional>
#include "log_stream.h"
#include "async_log.h"
#include "Timestamp.h"
class SourceFile
{
public:
    SourceFile(const char* file):_file(file)
    {
        const char* temp = strrchr(file, '/');
        if(temp){
            _file = temp + 1;
        }
        _size = static_cast<int>(strlen(_file));
    }

    const char* _file;
    int _size;
};




class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL, 
        NUM_LOG_LEVELS
    };
    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func_name);
    ~Logger();

    LogStream& stream();
    static LogLevel get_logLevel();
    static void setLogLevel(LogLevel level);
    static void setAsync();
    typedef std::function<void(const LogStream::Buffer&)> OutputFunc;
    static void setOutputFunc(OutputFunc func);

    class Impl
    {
    public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, const SourceFile& file, int line);

        void formatTime();
        void getThreadId();

        int64_t time_;
        LogStream stream_;
        LogLevel level_;
        SourceFile file_;
        int line_;
    };

private:
    Impl impl_;
    static bool is_async;
};



#define SET_LOGLEVEL(x) Logger::setLogLevel(x);

#define SET_LOG_ASYNC(x) \
    {   \
        if(x != 0){  \
            static AsyncLog g_async_;  \
            Logger::setOutputFunc( [&](const LogStream::Buffer& buf){ g_async_.append(buf.data(), buf.length());} );  \
            Logger::setAsync(); \
        }   \
    }   \

#define LOG_TRACE if(Logger::get_logLevel() <= Logger::TRACE) \
    (Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream())

#define LOG_DEBUG if(Logger::get_logLevel() <= Logger::DEBUG)   \
    (Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream())

#define LOG_INFO Logger(__FILE__, __LINE__, Logger::INFO).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
    
