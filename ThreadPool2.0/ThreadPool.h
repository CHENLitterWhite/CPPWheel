#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

typedef struct thread_pool_s thread_pool_t;

#ifdef __cplusplus
extern "C"
{
#endif

thread_pool_t *thread_pool_create(size_t thread_count);
void thread_pool_terminate(thread_pool_t *thread_pool);
int thread_pool_post(thread_pool_t *thread_pool, handler_pt func, void *arg);
void thread_pool_waitdone(thread_pool_t *thread_pool);

#ifdef __cplusplus
}
#endif


#endif