#include "second.h"

// g++ test.cpp -I./ -otestSec -Dcheck_mem_leak

int main()
{

    void *p1 = malloc(10);
    void *p2 = malloc(20);
    void *p3 = malloc(30);
    void *p4 = malloc(40);

    free(p1);
    free(p2);
    free(p3);
    free(p4);

    return 0;
}