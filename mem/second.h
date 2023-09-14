/*
    解决实际问题:
    1.检测是否发生内存泄漏
    2.内存泄漏的定位

    优势:
    1.用 __FILE__ __FUNCTION__  __LINE__   ==> 替代 addr2line 工具
    2.main中的malloc被替换成了malloc_def，而malloc_def中的malloc却没有被替换  ==> 通过宏函数的方法,避免hook可能带来的风险

    hook函数可能会带来一些风险:
    1.不可预测的行为。通过hook，我们可以修改、拦截甚至替换一些底层的系统调用、库函数等，从而导致代码行为的改变。
      这种改变可能会导致应用产生不可预测的行为，例如崩溃或者数据损坏等。
    2.安全问题。使用hook技术可以借助于一些恶意软件在不被用户察觉的情况下窃取或篡改数据、劫持用户操作、窃取用户密码等，从而导致个人隐私泄漏、财产受损等问题。
    3.兼容性问题。hook技术通常是通过修改已编译的可执行文件，插入新的代码、替换现有的代码等方式来实现。
      这种修改可能会导致应用程序在不同平台、不同版本的操作系统或库中出现不兼容性的问题。

    __FILE__      宏用于获取当前代码所在的文件名
    __FUNCTION__  宏用于获取当前代码所在的函数名
    __LINE__      宏用于获取当前代码所在的行号

    编译: -Dcheck_mem_leak
*/

// #define check_mem_leak         // 是否检测内存泄漏的宏

// 汇总结果

#ifdef __cplusplus
extern "C"
{
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

void* malloc_gc(size_t size, const char* file, const char* func, int line)
{
    void* p =malloc(size);

    char buff[128] = {0};
    sprintf(buff, "./mem/%p.log", p);

    FILE *fp = fopen(buff, "a+w");
    fprintf(fp, "ERROR [+%s:%s:%d] 内存泄漏 --> ADDR:%p, size:%ld\n", file, func, line, p, size);
    fflush(fp);  // 输出缓冲区的内容刷入到文件中，以确保程序输出的信息可以及时被显示
    fclose(fp);

    return p;
}

void free_gc(void* p, const char* file, const char* func, int line)
{
    char buff[128] = {0};
    sprintf(buff, "./mem/%p.log", p);

    if(unlink(buff) < 0)
    {
        FILE *fp = fopen(buff, "a+w");
        fprintf(fp, "ERROR [!%s:%s:%d] 二次释放 --> ADDR:%p\n", file, func, line, p);
        return;
    }

    free(p);
}

// 如果定义了check_mem_leak,则进行宏替换,否则使用原本的malloc
#ifdef check_mem_leak
#define malloc(size) malloc_gc(size, __FILE__, __FUNCTION__, __LINE__)
#define free(p) free_gc(p, __FILE__, __FUNCTION__, __LINE__);
#endif

}
#endif