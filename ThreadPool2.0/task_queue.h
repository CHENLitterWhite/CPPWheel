#ifndef _TASK_QUEUE_H_
#define _TASK_QUEUE_H_

typedef struct task_queue_s task_queue_t;
typedef void (*handler_pt)(void *);

// 可根据实际进行修改
typedef struct task_s
{
    void *next;
    
    handler_pt func;
    void *arg;

}task_t;

#ifdef __cplusplus
extern "C"
{
#endif

task_queue_t *task_queueu_create();
void nonblock(task_queue_t *task_queue);
void add_task(task_queue_t *task_queue, void *task);
void *pop_task(task_queue_t *task_queue);
void *get_task(task_queue_t *task_queue);
void task_queue_destroy(task_queue_t *task_queue);

#ifdef __cplusplus
}
#endif

#endif