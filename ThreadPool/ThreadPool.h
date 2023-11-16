//------------------------------------------//
// 作者: 干饭小白
// 时间: 2023-09-04
//------------------------------------------//
#pragma once


#ifdef __cplusplus
extern "C"
{
#endif __cplusplus

#define DEFAULT_TIME       5
#define MIN_WAIT_TASK_NUM  5
#define DEFAULT_THREAD_NUM 2

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

// 任务函数结构
typedef struct {
    void *(*function)(void*);
    void *arg;
}Task;

// 线程池管理者
struct Threadpool {

    /* 开关 */
    bool shutdown;

    /* 任务队列 */
    int queue_front;                  // 队头
    int queue_rear;                   // 队尾
    int queue_size;                   // 队列大小
    int queue_max_size;               // 最大的任务数
    Task *task_queue;                 // 任务队列

    /* 工作线程状态 */
    int min_thr_num;                  // 最小的线程数
    int max_thr_num;                  // 最大线线程数
    int live_thr_num;                 // 当前的活跃线程数
    int busy_thr_num;                 // 当前忙碌的线程数
    int wait_exit_thr_num;            // 当前正在等待退出的线程数
    pthread_t *threads;               // 线程集合

    /* 线程集合的管理 */
    pthread_t admin_tid;              // 线程管理者 -- 保持线程池中线程的相对平衡
    pthread_mutex_t lock;             // 互斥锁
    pthread_mutex_t thread_counter;   // 保证线程间竞争关系互斥 -- 正在忙碌的线程变量控制
    pthread_cond_t queue_not_full;    // 唤醒任务可入队线程 -- 队列不为满
    pthread_cond_t queue_not_empty;   // 唤醒工作线程取任务 -- 队列不为空
};

/*-----------------------------------------------
函数名:     
    threadpool_creat
函数说明:   
    创建并初始化一个线程池
输入参数:   
    min_thr_num(int)     最小的线程数量 
    max_thr_num(int)     最大的线程数量 
    queue_max_size(int)  任务队列大小
输出参数:
    无
返回值:
    返回线程池句柄,失败返回 NULL
-----------------------------------------------*/
Threadpool * threadpool_creat(int min_thr_num, int max_thr_num, int queue_max_size);

/*-----------------------------------------------
函数名:     
    threadpool_thread
函数说明:   
    任务处理线程函数
输入参数:   
    threadpool(Threadpool) 线程池句柄
输出参数:
    无
返回值:
    无
-----------------------------------------------*/
void * threadpool_thread(void *threadpool);

/*-----------------------------------------------
函数名:     
    threadpool_thread
函数说明:   
    任务处理线程函数
输入参数:   
    threadpool(Threadpool*) 线程池句柄
输出参数:
    无
返回值:
    无
-----------------------------------------------*/
void * admin_thread(void *threadpool);

/*-----------------------------------------------
函数名:     
    threadpool_free
函数说明:   
    释放资源
输入参数:   
    pool(Threadpool*) 线程池句柄
输出参数:
    无
返回值:
    0   正常
    -1  pool == NULL
-----------------------------------------------*/
int threadpool_free(Threadpool * pool);

/*-----------------------------------------------
函数名:     
    threadpool_destory
函数说明:   
    销毁线程池
输入参数:   
    pool(Threadpool*) 线程池句柄
输出参数:
    无
返回值:
    0   正常
    -1  pool == NULL
-----------------------------------------------*/
int threadpool_destory(Threadpool * pool);

/*-----------------------------------------------
函数名:     
    threadpool_add_task
函数说明:   
    向任务队列中添加事件
输入参数:   
    pool(Threadpool*) 线程池句柄
    void *(*function)(void *arg) 任务函数
    arg(void *) 任务函数携带的参数
输出参数:
    无
返回值:
    无
-----------------------------------------------*/
int threadpool_add_task(Threadpool * pool, void *(*function)(void *arg), void *arg);

/*-----------------------------------------------
函数名:     
    is_thread_alive
函数说明:   
    判断一个线程是否存活
输入参数:   
    tid(pthread_t) 线程pid
输出参数:
    无
返回值:
    存活 true
    消亡 false
-----------------------------------------------*/
bool is_thread_alive(pthread_t tid);

#ifdef __cplusplus
}
#endif


