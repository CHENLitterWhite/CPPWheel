#include "task_queue.h"
#include "lock/spinlock.h"
#include "lock/atomic.h"
#include <pthread.h>

typedef struct spinlock spinlock_t;



typedef struct task_queue_s
{

    void *head;
    void **tail;
    int block;
    spinlock_t lock;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

};

task_queue_t *
task_queueu_create(){

    int ret;

    task_queue_t *task_queue = (task_queue_t *)malloc(sizeof(task_queue_t));
    if(task_queue){

        ret = pthread_mutex_init(&task_queue->mutex, NULL);
        if(0 == ret){

            ret = pthread_cond_init(&task_queue->cond, NULL);
            if(0 == ret){
                spinlock_init(&task_queue->lock);
                task_queue->head = NULL;
                task_queue->tail = &task_queue->head;
                task_queue->block = 1;
                return task_queue;
            }

            pthread_mutex_destroy(&task_queue->mutex);
        }   

        free(task_queue);
    }

    return NULL;
}

void
nonblock(task_queue_t *task_queue){
    
    pthread_mutex_lock(&task_queue->mutex);
    task_queue->block = 0;
    pthread_mutex_unlock(&task_queue->mutex);
    pthread_cond_broadcast(&task_queue->cond);

}

void
add_task(task_queue_t *task_queue, void *task){

    void **link = (void **)task;
    *link = NULL;

    spinlock_lock(&task_queue->lock);
    *task_queue->tail = link;
    task_queue->tail = link;
    spinlock_unlock(&task_queue->lock);
    pthread_cond_signal(&task_queue->cond);
}

void *
pop_task(task_queue_t *task_queue){

    spinlock_lock(&task_queue->lock);
    if(task_queue->head == NULL){
        spinlock_unlock(&task_queue->lock);
        return NULL;
    }

    task_t *task;
    task = task_queue->head;

    void **link = (void **)task;
    task_queue->head = *link;

    // 如果队列此时为空,需要置回tail
    if(task_queue->head == NULL){
        task_queue->tail = &task_queue->head;
    }

    spinlock_unlock(&task_queue->lock);

    return task;
}

void *
get_task(task_queue_t *task_queue){

    task_t *task;

    // C++条件变量存在虚假唤醒
    while((task = __pop_task(task_queue)) == NULL){
        pthread_mutex_lock(&task_queue->mutex);
        if(task_queue->block == 0){
            pthread_mutex_unlock(&task_queue->mutex);
            return NULL;
        }

        // 内部：先解锁 unlock(&mutex); 
        // 休眠
        // 唤醒
        // 加锁 lock(&mutex)
        pthread_cond_wait(&task_queue->cond, &task_queue->mutex);
        pthread_mutex_unlock(&task_queue->mutex);
    }

    return task;
}

inline void
task_queue_destroy(task_queue_t *task_queue){

    task_t *task;
    
    while((task == __pop_task(task_queue))){
        free(task);
    }

    spinlock_destroy(&task_queue->lock);
    pthread_mutex_destroy(&task_queue->mutex);
    pthread_cond_destroy(&task_queue->cond);

    free(task_queue);
}