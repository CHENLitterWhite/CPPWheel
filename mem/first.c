
#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <link.h>
#include <unistd.h>

// void *malloc (size_t __size)     
typedef void *(*malloc_t)(size_t);
malloc_t malloc_f;                // ---> 通过函数指针指向 malloc

// void free (void *__ptr)
typedef void (*free_t)(void *);
free_t free_f;                    // ---> 通过函数指针指向 free

// “hook”一词，它是指对一个函数进行修改或拦截，使其执行不同于原始实现的操作
static int init_hook()
{
    // # define RTLD_NEXT	((void *) -1l
    malloc_f = dlsym(RTLD_NEXT, "malloc");
    free_f = dlsym(RTLD_NEXT, "free");

    return 0;
}

int num = 0;
void *malloc(size_t size) {
    printf("In malloc\n");            // 一直在里面递归:说明printf()也调用了malloc
    return NULL;
}

void free(void *ptr) {
    printf("In free\n");
}

int main(void)
{
    init_hook();
    void *p1 = malloc(10);
    void *p2 = malloc(10);

    free(p1);

    return 0;
}
