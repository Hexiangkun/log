#include "log_file.h"
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>

FileWriter::FileWriter(std::string file_name):written_bytes(0)
{
    _file = fopen(file_name.c_str(), "a+");//打开文件
    if(!_file){
        return;
    }
    ::setbuffer(_file, _buffer, sizeof(_buffer));//设置文件缓冲区
    /*
    setbuffer可以将一个指定大小的缓冲区和文件流相关，以提高读写效率。
    数据先被写入缓冲区中，当缓冲区满了或者显示调用fflush函数时，缓冲区的数据才会被写入到实际文件中。
    减少访问文件的次数，提高读写效率。
    */
}

FileWriter::~FileWriter()
{
    if(_file){
        fclose(_file);
    }
}

off_t FileWriter::get_written_bytes() const
{
    return written_bytes;
}

void FileWriter::append(const char* line,size_t len)//将给定长度的字符串写入文件缓冲区
{
    size_t n = ::fwrite(line, 1,len, _file);    //写入_buffer缓冲区
    size_t remain = len-n;
    while(remain){
        size_t x = fwrite(line+n, 1, remain, _file);
        if(x == 0){
            int err = ferror(_file);
            if(err){
                fprintf(stderr, "FileWriter::append() failed %d\n",err);
            }
        }
        n += x;
        remain -= x;
    }
    written_bytes += len;
}

void FileWriter::flush()//将缓冲区写入文件，并清空缓冲区
{
    fflush(_file);
}

LogFile::LogFile(off_t roll_size):roll_size(roll_size),file_index(0)
{
    setBaseName();
    rollFile();
}

LogFile::~LogFile()=default;

void LogFile::rollFile()
{
    std::string file_name = getLogFileName();
    _file.reset(new FileWriter(file_name));//创建新的Filewriter对象
    unlink(linkname);//删除linkname指定的文件,删除软连接

    symlink(file_name.c_str(), linkname);//重新创建软连接

}
//设置日志文件名称
void LogFile::setBaseName()
{
    char log_abs_path[PATH_MAX] = {0};
    //
    char *res = getcwd(log_abs_path, sizeof(log_abs_path));//获取当前程序所在目录
    if(!res){
        fprintf(stderr,"getcwd error\n");
    }
    strcat(log_abs_path, "/log/"); 
    if(access(log_abs_path, 0)== -1){   //检查目录是否存在
        mkdir(log_abs_path, 0755);  //不存在，创建目录
    }

    char process_abs_path[PATH_MAX] ={0};
    //获取当前程序所在位置，包含程序名
    long len = readlink("/proc/self/exe", process_abs_path, sizeof(process_abs_path));//读取当前程序的绝对路径，也就是可执行文件的名称
    if(len <= 0){
        return ;
    }

    char* process_name = strrchr(process_abs_path, '/')+1;  //在process_abs_path中查找最后一个斜杠/，获取可执行文件名称
    snprintf(linkname, sizeof(linkname), "%s%s.log", log_abs_path, process_name);
    snprintf(basename, sizeof(basename), "%s%s.%d.", log_abs_path,process_name, getpid());
    
}

std::string LogFile::getLogFileName()
{
    std::string file_name(basename);
    char timebuf[32] = {0};
    struct tm tm;
    time_t now ;
    time(&now);
    localtime_r(&now, &tm);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S.", &tm);
    file_name += timebuf;

    char index[8] ={0};
    snprintf(index, sizeof(index), "%03d.log", file_index);
    ++file_index;
    file_name += index;
    return file_name;
}

void LogFile::append(const char* line, const size_t len)//追加日志信息
{
    std::unique_lock<std::mutex> lock(_mutex);
    _file->append(line, len);

    if(_file->get_written_bytes() > roll_size){//如果日志文件大小超过设定的roll_size，重新创建日志文件
        rollFile();
    }
}

void LogFile::flush()//将缓存数据全部写入磁盘中去
{
    std::unique_lock<std::mutex> lock(_mutex);
    _file->flush();
}