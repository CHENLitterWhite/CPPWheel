#include "ThreadPool.h"

Threadpool * threadpool_creat(int min_thr_num, int max_thr_num, int queue_max_size)
{
    Threadpool *pool = NULL;
    
    /* 使用do{}while(0) --> 实现goto机制,错误时即使跳出,并统一处理 */
    do
    {
        /** 开启线程池空间 **/
        pool = (Threadpool *)malloc(sizeof(Threadpool));
        if(NULL == pool)
        {
            break;
        }

        /** 初始化信息 **/
        pool->min_thr_num = min_thr_num;
        pool->max_thr_num = max_thr_num;
        pool->live_thr_num = min_thr_num;
        pool->busy_thr_num = 0;
        pool->wait_exit_thr_num = 0;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->queue_size = 0;
        pool->queue_max_size = queue_max_size;
        pool->shutdown = false;

        /** 分配工作线程空间 **/
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * max_thr_num);
        if(NULL == pool->threads)
        {
            break;
        }
        memset(pool->threads, 0, sizeof(pthread_t) * max_thr_num);

        /** 队列空间 **/
        pool->task_queue = (Task *)malloc(sizeof(Task) * queue_max_size);
        if(NULL == pool->task_queue)
        {
            break;
        }

        /** 初始化互斥锁和条件变量 **/
        if(pthread_mutex_init(&(pool->lock), NULL) != 0 ||
           pthread_mutex_init(&(pool->thread_counter), NULL) != 0 ||
           pthread_cond_init(&(pool->queue_not_empty), NULL) ||
           pthread_cond_init(&(pool->queue_not_full), NULL) != 0)
        {

            break;
        } 

        /** 启动 min_thr_num 个工作线程 **/
        for(int i = 0; i < min_thr_num; ++i)
        {
            pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
        }

        /** 启动管理者线程 **/
        pthread_create(&(pool->admin_tid), NULL, admin_thread, (void*)pool);

        return pool;

    }while(0);

    /* 释放pool的空间 */
    threadpool_free(pool);

    return NULL;
}

int threadpool_destory(Threadpool * pool)
{
    /* 容错性判断 */
    if(NULL == pool)
    {
        return -1;
    }

    /* 线程标志位:true */
    pool->shutdown = true;

    /* 销毁管理线程 */
    pthread_join(pool->admin_tid, NULL);

    /* 通知存活的线程结束自己 */
    for(int i = 0; i < pool->live_thr_num; ++i)
    {
        /** 唤醒所有被阻塞的线程 **/
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }

    /* 等待线程结束,进行回收 */
    for(int i = 0; i < pool->live_thr_num; ++i)
    {
        pthread_join(pool->threads[i], NULL);
    }

    /* 释放资源 */
    threadpool_free(pool);

    return 0;
}

int threadpool_free(Threadpool * pool)
{
    /* 容错性判断 */
    if(NULL == pool)
    {
        return -1;
    }

    /* 销毁任务队列 */
    if(pool->task_queue)
    {
        free(pool->task_queue);
        pool->task_queue = NULL;
    }

    /* 销毁工作线程集合 */
    if(pool->threads)
    {
        free(pool->threads);
        pool->threads = NULL;
    }

    /* 销毁池管理空间 */
    free(pool);
    pool = NULL;

    return 0;
}

void * admin_thread(void *threadpool)
{
    /* 容错处理 */
    if(NULL == threadpool)
    {
        return NULL;
    }

    /* 维护线程池的平衡 */
    Threadpool *pool = (Threadpool *)threadpool;
    while(!pool->shutdown)
    {

        /** 每隔DEFAULT_TIME进行一次 **/
        sleep(DEFAULT_TIME);

        /** 获取当前状态下活跃数和任务数 -- 多线程访问同一个变量(互斥锁) **/
        pthread_mutex_lock(&(pool->lock));
        int queue_size = pool->queue_size;
        int live_thr_num = pool->live_thr_num;
        pthread_mutex_unlock(&(pool->lock));

        /** 获取忙碌数 **/
        pthread_mutex_lock(&(pool->thread_counter));
        int busy_thr_num = pool->busy_thr_num;
        pthread_mutex_unlock(&(pool->thread_counter));

        /** 创建新线程:正在等待的任务 >= 最小允许等待的任务数 && 存活线程数 < 最大线程数 **/
        if(queue_size >= MIN_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num)
        {
            pthread_mutex_lock(&(pool->lock));
            int add = 0;
            for(int i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_NUM && pool->live_thr_num < pool->max_thr_num; ++i)
            {
                if(pool->threads[i] == 0 || !is_thread_alive(pool->threads[i]))
                {
                    pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void*)pool);
                    add++;
                    pool->live_thr_num++;
                }
            }
            pthread_mutex_unlock(&(pool->lock));
        }

        /** 销毁空闲的线程: 忙碌数*2 < 存活数(一半以上的空闲) && 存活数 > 最小线程数 **/
        if((busy_thr_num * 2) < live_thr_num && live_thr_num > pool->min_thr_num)
        {
            pthread_mutex_lock(&(pool->lock));
            pool->wait_exit_thr_num = DEFAULT_THREAD_NUM;
            pthread_mutex_unlock(&(pool->lock));
        }

        for(int i = 0; i < DEFAULT_THREAD_NUM; ++i)
        {
            /*** 通知处于空闲的线程自杀 ***/
            pthread_cond_signal(&(pool->queue_not_empty));
        }

    }

    return NULL;
}

bool is_thread_alive(pthread_t tid)
{
    int kill_rc = pthread_kill(tid, 0);
    if(ESRCH == kill_rc)
    {
        return false;
    }

    return true;
}

void * threadpool_thread(void *threadpool)
{
    /* 容错处理 */
    if(NULL == threadpool)
    {
        return NULL;
    }

    Threadpool *pool = (Threadpool *)threadpool;
    Task task;

    while(true)
    {
        pthread_mutex_lock(&(pool->lock));

        /* 无任务:阻塞等待 */
        while((pool->queue_size == 0 && !pool->shutdown))
        {
            /** 阻塞等待 **/
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));

            /** 是否是自杀任务 **/
            if(pool->wait_exit_thr_num > 0)
            {
                pool->wait_exit_thr_num--;

                /** 判断线程池中的线程数量是否大于最小线程数:是则结束当前线程 **/
                if(pool->live_thr_num > pool->min_thr_num)
                {
                    pool->live_thr_num--;
                    pthread_mutex_unlock(&(pool->lock));
                    pthread_exit(NULL);
                }
            }
        }

        /* 是否销毁池 */
        if(pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->lock));
            pthread_exit(NULL);
        }

        /* 有任务:执行任务 */
        task.function = pool->task_queue[pool->queue_front].function;
        task.arg = pool->task_queue[pool->queue_front].arg;

        /* 维护循环队列平衡 */
        pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size;
        pool->queue_size--; 
        
        /* 通知队列不未满 */
        pthread_cond_broadcast(&(pool->queue_not_full));
        
        pthread_mutex_unlock(&(pool->lock));

        /* 执行开始:忙碌线程+1 */
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num++;
        pthread_mutex_unlock(&(pool->thread_counter));
        
        /* 执行任务 */
        (*(task.function))(task.arg);

        /* 执行结束:忙碌线程-1 */
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num--;
        pthread_mutex_unlock(&(pool->thread_counter));
    }

    pthread_exit(NULL);
}

int threadpool_add_task(Threadpool * pool, void *(*function)(void *arg), void *arg)
{
    if(NULL == pool)
    {
        return -1;
    }
    
    pthread_mutex_lock(&(pool->lock));

    /* 任务队列满时需要等到 */
    while((pool->queue_size == pool->queue_max_size) && (!pool->shutdown))
    {
        pthread_cond_wait(&(pool->queue_not_full), &(pool->lock));
    }

    /* 线程池处于关闭状态 */
    if(pool->shutdown)
    {
        pthread_mutex_unlock(&(pool->lock));
    }

    /* 清空残留 */
    if(pool->task_queue[pool->queue_rear].arg != NULL)
    {
        free(pool->task_queue[pool->queue_rear].arg);
        pool->task_queue[pool->queue_rear].arg = NULL;
    }

    /* 添加任务 */
    pthread_cond_signal(&(pool->queue_not_empty));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}