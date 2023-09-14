/*
__libc_malloc 和_  _libc_free
它们是malloc和free底层调用的函数
*/

#define _GNU_SOURCE

// #include <cstddef>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <link.h>
#include <unistd.h>

// extern void* __libc_malloc(size_t size);
// extern void __libc_free(void *ptr);

// 使用 GCC 自带的函数 __builtin_return_address() 和 __builtin_frame_address()，可以获取当前函数的返回地址和帧地址。
// 进一步地，我们可以计算该帧地址的上一级（caller）函数的地址，并使用 dladdr() 函数检索出函数符号以获取函数名称。

// void *malloc (size_t __size)     
typedef void *(*malloc_t)(size_t);
int enable_malloc_hook = 1;
malloc_t malloc_f;                // ---> 通过函数指针指向 __libc_malloc

// void free (void *__ptr)
typedef void (*free_t)(void *);
int enable_free_hook = 1;
free_t free_f;                    // ---> 通过函数指针指向 __libc_free

// “hook”一词，它是指对一个函数进行修改或拦截，使其执行不同于原始实现的操作
static int init_hook()
{
    // # define RTLD_NEXT	((void *) -1l
    malloc_f = dlsym(RTLD_NEXT, "malloc");
    free_f = dlsym(RTLD_NEXT, "free");

    return 0;
}

void *malloc(size_t size) {
    if(enable_malloc_hook)
    {
        enable_malloc_hook = 0;
        // void* p = __libc_malloc(size);
        void* p = malloc_f(size);
        printf("In malloc %p %ld\n", p, size);            // 一直在里面递归:说明printf()也调用了malloc

        enable_malloc_hook  = 1;
        return p;

    }
    else 
    {
        return malloc_f(size);
    }
   
    return NULL;
}

void free(void *ptr) 
{
    if(enable_free_hook)
    {
        enable_free_hook = 0;
        printf("In free %p\n", ptr);

        // __libc_free(ptr);
        free_f(ptr);
        enable_free_hook = 1;
    }
    else 
    {
        return free_f(ptr);
    }
}

int main(void)
{
    init_hook();
    void *p1 = malloc(10);
    void *p2 = malloc(10);

    free(p1);
    free(p2);

    return 0;
}
