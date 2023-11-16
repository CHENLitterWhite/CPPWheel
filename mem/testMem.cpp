#include "mem.h"

int main()
{
    init_gc("./mem", OPEN_TOTAL_INFO | OPEN_SINGLE_INFO | OPEN_THREAD_INFO);
    
    void* p1 = malloc(1);
    void* p2 = malloc(1);
    void* p3 = malloc(1);

    free(p1);
    free(p2);
    // free(p3);
 
    total_gc();
  
    return 0;
}
