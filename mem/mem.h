/*
*    作者: 干饭小白
*    时间: 2023-09-25 16:00:00:00
*    
*    说明:
*      只能检测 malloc 和 free,无法检测 new delete
*/
#pragma once

// #define CHECKMEM

#define OPEN_TOTAL_INFO    0x001
#define OPEN_SINGLE_INFO   0x010
#define OPEN_THREAD_INFO   0x100

#include <map>
#include <string>
#include <cstddef>
#include <utility>

#ifdef __cplusplus
extern "C"
{
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
};
#endif

struct _gc_info
{
  long _size;
  std::string _content;
  _gc_info()
  {

  }
  _gc_info(long size, std::string con)
  {
    _size = size;
    _content = con;
  }
};

static char Logpath[256] = {0};
static char DirPath[256] = {0};
static std::map<std::string ,_gc_info> total;
volatile static long totalMalloc = 0;
volatile static long totalFree = 0;
volatile static long totalMallocSize = 0;
volatile static long totalFreeSize = 0;
static uint16_t Mode = false;

static sem_t sem_mutex;

extern void get_time(char* str)
{
  time_t now;
  struct tm *local_time;

  now = time(NULL);
  local_time = localtime(&now);

  sprintf(str, "%d-%02d-%02d_%02d:%02d:%02d", 
            local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
            local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
  
  return;
}

extern void init_gc(const char* dirPath, uint16_t mode = OPEN_TOTAL_INFO)
{

    sem_init(&sem_mutex, 0, 1);

    struct stat st;
    if (stat(dirPath, &st) == 0 && S_ISDIR(st.st_mode))
    {
        // printf("文件夹已存在\n");
    }
    else 
    {
        if(mkdir(dirPath, 0777) != 0)
        {
            return;
        } 
    }
    
    Mode = mode;
    strncpy(DirPath, dirPath, 256);
    
    char time[256] = {0};
    get_time(time);
    sprintf(Logpath, "%s/%s_total.log", dirPath, time);
    unlink(Logpath);

    char buff[128] = {0};
    sprintf(buff, "%s/thread.log", DirPath);  
    unlink(buff);

    return;
}

extern void total_gc()
{

    sem_destroy (&sem_mutex);

    if((Mode & OPEN_TOTAL_INFO) == OPEN_TOTAL_INFO)
    {
        FILE *fp = fopen(Logpath, "a+w");
        fprintf(fp, "\n=========================================\n");
        fprintf(fp, "申请次数:%ld\n", totalMalloc);
        fprintf(fp, "释放次数:%ld\n", totalFree);
        fprintf(fp, "申请总大小:%ld\n", totalMallocSize);
        fprintf(fp, "释放总大小:%ld\n", totalFreeSize);
        fprintf(fp, "未释放总大小:%ld\n", totalMallocSize - totalFreeSize);
        fprintf(fp, "问题统计:%ld\n", total.size());
        fprintf(fp, "=========================================\n");
        fflush(fp);

        for(auto it = total.begin(); it != total.end(); ++it)
        {
            fprintf(fp, "%s", it->second._content.c_str());
            fflush(fp);
        }

        fclose(fp);
    }

    total.clear();
}

extern void* malloc_gc(size_t size, const char* file, const char* func, int line)
{
    sem_wait(&sem_mutex);

    FILE *test = NULL;
    if((Mode & OPEN_THREAD_INFO) == OPEN_THREAD_INFO)
    {   
        char buff[128] = {0};
        sprintf(buff, "%s/thread.log", DirPath);
        test = fopen(buff, "a+w");
        fprintf(test, "ERROR 进入malloc_gc--->%ld\n", pthread_self());
        fflush(test);  
    }

    void* p =malloc(size);
    char time[256] = {0};
    get_time(time);

    FILE *fp = NULL;
    if((Mode & OPEN_SINGLE_INFO) == OPEN_SINGLE_INFO)
    {
        char buff[128] = {0};
        sprintf(buff, "%s/%p.log", DirPath,p);
        FILE *fp = fopen(buff, "w");
        fprintf(fp, "%s:ERROR [+%s:%s:%d][PID:%ld] --> ADDR:%p, size:%ld\n", time, file, func, line, pthread_self() , p, size);
        fflush(fp);
        fclose(fp);
    }
   
    if((Mode & OPEN_TOTAL_INFO) == OPEN_TOTAL_INFO)
    {
        totalMallocSize+= size;
        fp = fopen(Logpath, "a+w");
        fprintf(fp, "%s:[+%s:%s:%d][PID:%ld] --> ADDR:%p, size:%ld\n", time, file, func, line, pthread_self(), p, size);
        fflush(fp);
        fclose(fp);

        char content[256] = {0};
        sprintf(content, "%s:ERROR [+!%s:%s:%d][PID:%ld] 内存泄漏 --> ADDR:%p, size:%ld\n", time, file, func, line, pthread_self(), p, size);
        char addr[128] = {0};
        sprintf(addr, "%p", p);
        _gc_info info(size, content);
        total.insert(std::make_pair(addr, info));

        totalMalloc++;
    }
   
    if((Mode & OPEN_THREAD_INFO) == OPEN_THREAD_INFO)
    { 
        fprintf(test, "INFO  离开malloc_gc--->%ld\n", pthread_self());
        fflush(test);  
        fclose(test);
    }

    sem_post(&sem_mutex);

    return p;
}

extern void free_gc(void* p, const char* file, const char* func, int line)
{
    sem_wait(&sem_mutex);  

    FILE *test = NULL;
    if((Mode & OPEN_THREAD_INFO) == OPEN_THREAD_INFO)
    { 
        char buff[128] = {0};
        sprintf(buff, "%s/thread.log", DirPath);
        test = fopen(buff, "a+w");
        fprintf(test, "ERROR 进入free_gc--->%ld\n", pthread_self());
        fflush(test);  
    }
    
    char time[256] = {0};
    get_time(time);

    if((Mode & OPEN_TOTAL_INFO) == OPEN_TOTAL_INFO)
    {
        totalFree++;
        char addr[128] = {0};
        sprintf(addr, "%p", p);
        if(total.find(addr) == total.end())
        {
            char content[256] = {0};
            sprintf(content, "%s:ERROR [-!%s:%s:%d][PID:%ld] 二次释放 --> ADDR:%p\n", time, file, func, line, pthread_self(), p);
            char addr[128] = {0};
            sprintf(addr, "%p", p);
            _gc_info info(0, content);
            sem_post(&sem_mutex);
            total.insert(std::make_pair(addr, info));
        }
        else 
        {
            totalFreeSize += total[addr]._size;
            FILE* fp = fopen(Logpath, "a+w");
            fprintf(fp, "%s:[-%s:%s:%d][PID:%ld] --> ADDR:%p, size:%ld\n", time, file, func, line, pthread_self(), p, total[addr]._size);
            fflush(fp);
            fclose(fp);

            total.erase(addr);
            free(p);
        }
    }

    char buff[128] = {0};
    sprintf(buff, "%s/%p.log", DirPath,p);
    if((Mode & OPEN_SINGLE_INFO) == OPEN_SINGLE_INFO && unlink(buff) < 0)
    {
        FILE *fp = fopen(buff, "w");
        fprintf(fp, "%s:ERROR [-!%s:%s:%d][PID:%ld] 二次释放 --> ADDR:%p\n", time, file, func, line, pthread_self(), p);
        fflush(fp);
        fclose(fp);
    }
  
    if((Mode & OPEN_THREAD_INFO) == OPEN_THREAD_INFO)
    {
        fprintf(test, "INFO  离开free_gc--->%ld\n", pthread_self());
        fflush(test);  
        fclose(test);
    }

    sem_post(&sem_mutex);

    return;
}


#ifdef CHECKMEM
#define malloc(size) malloc_gc(size, __FILE__, __FUNCTION__, __LINE__)
#define free(p) free_gc(p, __FILE__, __FUNCTION__, __LINE__)
#endif