#include "ThreadPool.h"
#include "task_queue.h"
#include <stdatomic.h>
#include <pthread.h>

struct thread_pool_s
{
    task_queue_t *task_queue;
    atomic_int quit;
    int thread_count;
    pthread_t *threads;
};

static void
__thrdpool_worker(void *arg){

    thread_pool_t *thread_pool = (thread_pool_t *)arg;
    task_t *task;
    void *ctx;

    while(atomic_load(&thread_pool->quit) == 0){

        task = (task_t *) get_task(thread_pool->task_queue);
        
        if(!task){
            break;
        }

        handler_pt func = task->func;
        ctx = task->arg;
        func(ctx);
    }

    return NULL;
}

static void 
__threads_terminate(thread_pool_t * thread_pool) {
    atomic_store(&thread_pool->quit, 1);
    __nonblock(thread_pool->task_queue);
    int i;
    for (i=0; i<thread_pool->thread_count; i++) {
        pthread_join(thread_pool->threads[i], NULL);
    }
}


static int
__thread_create(thread_pool_t *thread_pool, size_t thread_count){

    int ret;
    pthread_attr_t attr;

    ret = pthread_attr_init(&attr);

    if(0 == ret)
    {
        thread_pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
        if(thread_pool->threads){
            int i = 0;
            for(; i < thread_count; ++i){
                if(pthread_create(&thread_pool->threads[i], &attr, __thrdpool_worker, thread_pool) != 0){
                    break;
                }
            }

            thread_pool->thread_count = i;
            pthread_attr_destroy(&attr);

            if(i == thread_count){
                return 0;
            }

            __threads_terminate(thread_pool);
            free(thread_pool->threads);
        }

        return -1;
    }

    return ret;
}

thread_pool_t *
thread_pool_create(size_t thread_count){

    thread_pool_t *thread_pool;

    thread_pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    if(thread_pool)
    {   
        task_queue_t *task_queue = task_queueu_create();
        if(task_queue){

            thread_pool->task_queue = task_queue;
            atomic_init(&thread_pool->quit, 0);
            if(__thread_create(thread_pool, thread_count) == 0){
                return thread_pool;
            }

            task_queue_destroy(task_queue);
        }

        free(thread_pool);
    }

    return NULL;
}

void 
thread_pool_terminate(thread_pool_t *thread_pool){

    atomic_store(&thread_pool->quit, 1);
    __nonblock(thread_pool->task_queue);
    int i;
    for (i=0; i < thread_pool->thread_count; ++i) {
        pthread_join(thread_pool->threads[i], NULL);
    }

}

void 
thread_pool_waitdone(thread_pool_t *thread_pool){

    int i;
    for(i = 0; i < thread_pool->thread_count; ++i){
        pthread_join(thread_pool->threads[i], NULL);
    }   

    task_queue_destroy(thread_pool->task_queue);
    free(thread_pool->threads);
    free(thread_pool);
}

int 
thread_pool_post(thread_pool_t *thread_pool, handler_pt func, void *arg){

    if (atomic_load(&thread_pool->quit) == 1){
        return -1;
    }
       
    task_t *task = (task_t*) malloc(sizeof(task_t));
    if (!task){
        return -1;
    } 

    task->func = func;
    task->arg = arg;

    add_task(thread_pool->task_queue, task);

    return 0;
}