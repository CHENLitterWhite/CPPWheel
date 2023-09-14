#include <shmfifo.h>
#include <sys/shm.h>
#include <unistd.h>

int main()
{

    pid_t pid;
    int i = 0;
    // 创建5进程,同时写
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
        CShmFIFO shmFifo;
        shmFifo.Connect(0x001);
        char buf[256] = "aaaaaaa";

        int num = 0;
        while(1)
        {
            shmFifo.Write(buf, strlen(buf));
            num++;
        }

        shmFifo.Separate();
    }
    else if(i == 5)
    {
        CShmFIFO shmFifo;
        shmFifo.Connect(0x001);
        char buf[256] = "bbbbbbbbb";
        int num = 0;
        while(1)
        {
            shmFifo.Write(buf, strlen(buf));
        }

        shmFifo.Separate();
    }

    return 0;
}