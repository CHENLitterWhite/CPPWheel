#include "shmfifo.h"
#include <cstring>
#include <stdio.h>
#include <sys/ipc.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();
    int i = 0;
    for(i = 0; i < 5; ++i)
    {
        pid = fork();

        if(pid == 0)
        {
            break;
        }
    }
    
    if(i < 5)
    {
        sleep(3);
        CShmFIFO shmFifo;
        shmFifo.Connect(0x001);
        char buf[256] = {0};

        int num = 0;
        while(1)
        {
            shmFifo.Read(buf, 256);
            printf("%s\n", buf);
            memset(buf, 0, 256);
        }

        shmFifo.Separate();
    }
    else if(i == 5)
    {
        CShmFIFO shmFifo;
        char buf[256] = {0};

        if(!shmFifo.Init(0x001, 256, 5))
        {
            printf("Init err\n");
            return 0;
        }

        int num = 0;
        while(1)
        {
            shmFifo.Read(buf, 256);
            printf("%s\n", buf);
            memset(buf, 0, 256);
            // num++;
        }

        shmFifo.Distory(100);
    }
 
    return 0;
}