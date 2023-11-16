#include <stdio.h>

#ifdef RINGBUFFER
#include <RingBuffer.h>
#endif

int main()
{
   
    #ifdef RINGBUFFER
    print();
    #endif
    

    return 0;
}