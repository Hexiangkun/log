#include "log.h"
#include <sys/syscall.h>
#include <unistd.h>

Logger::LogLevel g_log_level = Logger::INFO;

Logger::LogLevel Logger::get_logLevel()
{
    return g_log_level;
}

bool g_is_async = false;
void Logger::setAsync()
{
    g_is_async = true;
}

void Logger::setLogLevel(LogLevel level)
{
    g_log_level = level;
}

class T
{
    public:
        T(const char* str, int len):str_(str), len_(len){}
        const char* str_;
        const int len_;
};

LogStream& Logger::stream()
{
    return impl_.stream_;
}

LogStream& operator<<(LogStream& s, T v)
{
    s.append(v.str_,v.len_);
    return s;
}

LogStream& operator<<(LogStream& s, SourceFile v)
{
    s.append(v._file, v._size);
    return s;
}

void defaultOutput(const LogStream::Buffer& buf)//默认向终端写日志
{
    size_t n = fwrite(buf.data(), 1, static_cast<size_t>(buf.length()), stdout);
}

Logger::OutputFunc g_output_func = defaultOutput;

void Logger::setOutputFunc(OutputFunc func)
{
    g_output_func = func;
}

Logger::Logger(SourceFile file, int line):impl_(INFO,file,line)
{

}

Logger::Logger(SourceFile file, int line, LogLevel level) :impl_(level, file, line)
{

}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func_name):impl_(level, file, line)
{
    impl_.stream_ << func_name << ' ';
}


Logger::~Logger()
{
    stream()<< "\n";
    const LogStream::Buffer& buf(stream().get_buffer());
    g_output_func(buf);
}

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

Logger::Impl::Impl(LogLevel level, const SourceFile& file, int line):time_(Timestamp::now()),
            stream_(),level_(level),file_(file),line_(line)
            {
                formatTime();
                getThreadId();

                stream_ << T(LogLevelName[level], 6);
                stream_ << file_ <<" : " << line_ << " -> ";
            }

void Logger::Impl::formatTime()
{
    time_t seconds = static_cast<time_t>(time_ / Timestamp::micro_sec_persec);
    int micro_seconds = static_cast<int> (time_ % Timestamp::micro_sec_persec);

    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);
    char t_time[64]={0};
    
    snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour + 8, tm_time.tm_min, tm_time.tm_sec);
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), ".%03d", micro_seconds);
    stream_ << T(t_time, 19) << T(buf, 4);
}

void Logger::Impl::getThreadId()
{
    char buf[32] = {0};
    int length = snprintf(buf, sizeof(buf), " %5lu ", syscall(SYS_gettid));
    stream_ << T(buf, length);
}