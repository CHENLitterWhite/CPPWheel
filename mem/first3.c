#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <link.h>
#include <unistd.h>

typedef void *(*malloc_t)(size_t);
int enable_malloc_hook = 1;
malloc_t malloc_f;

typedef void (*free_t)(void *);
int enable_free_hook = 1;
free_t free_f;  

static int init_hook()
{
    malloc_f = dlsym(RTLD_NEXT, "malloc");
    free_f = dlsym(RTLD_NEXT, "free");

    return 0;
}

void *ConvertToVMA(void *addr) {
    Dl_info info;
    struct link_map *link_map;
    dladdr1((void *) addr, &info, (void **) &link_map, RTLD_DL_LINKMAP);
    return addr - link_map->l_addr;
}

void *malloc(size_t size) {
    if(enable_malloc_hook)
    {
        enable_malloc_hook = 0;
        // void* p = __libc_malloc(size);
        void* p = malloc_f(size);

        // void* caller = ConvertToVMA(__builtin_return_address(0));
        void* caller = ConvertToVMA(__builtin_return_address(0));
        printf("[+%p]--->ptr:%p size:%zu\n", caller, p, size);          // 一直在里面递归:说明printf()也调用了malloc

        char command[256];
        Dl_info info;
        dladdr(malloc, &info);

        snprintf(command, sizeof(command), "addr2line -f -e %s -a %p > ./mem/%p.mem", info.dli_fname, caller, p);
        system(command);

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
        void *caller = __builtin_return_address(0);
        printf("[-%p]--->ptr:%p\n", caller, ptr);

        char buff[128] = {0};
        sprintf(buff, "./mem/%p.mem", ptr);
        if (unlink(buff) < 0) {
            printf("double kill:%p\n",ptr);
        }

        // __libc_free(ptr);
        free_f(ptr);
        enable_free_hook = 1;
    }
    else 
    {
        return free_f(ptr);
    }
}

int main()
{

    init_hook();

    void *p1 = malloc(10);
    void *p2 = malloc(20);

    free(p1);

    return 0;
}
