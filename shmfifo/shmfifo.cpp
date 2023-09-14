#include "shmfifo.h"
#include <cstddef>
#include <cstring>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

CShmFIFO::CShmFIFO():m_shmHead(NULL),
                     m_payload(NULL)
{

}

CShmFIFO::~CShmFIFO()
{

}

bool CShmFIFO::Init(key_t key, uint16_t blockSize, uint16_t blockNum)
{
    do 
    {
        int shmid = 0;

        // 如果存在则销毁,并重新创建
        shmid = shmget(key, 0, IPC_EXCL);
        if(shmid != -1)
        {
            if(shmctl(shmid, IPC_RMID, 0) == -1)		//删除共享内存
            {
                break;
            }
        }

        // 创建共享内存 -- 如果存在会创建失败
        shmid = shmget(key, sizeof(shmHead_t) + blockSize * blockNum, IPC_CREAT | IPC_EXCL | 0666);
        if(shmid == -1)
        {
            break;
        }

        // 连接 -- 只做初始化
        m_shmHead = shmat(shmid, NULL, 0);
        if(m_shmHead == (void *)-1)
        {
            break;
        }
        memset(m_shmHead, 0, sizeof(shmHead_t) + blockSize * blockNum);

        shmHead_t* pHead = (shmHead_t*)m_shmHead;
        pHead->shmid     = shmid;
        pHead->blockSize = blockSize;
        pHead->blockNum  = blockNum;
        pHead->readIndex = 0;
        pHead->writeIndex = 0;

        sem_init(&pHead->sem_mutex, 1, 1);
        sem_init(&pHead->sem_empty, 1, 0);
        sem_init(&pHead->sem_full, 1, blockNum);

        m_payload = (char *)(pHead + 1);

    }while (0);


    if(m_payload == NULL)
    {
        return false;
    }

    return true;
}

bool CShmFIFO::Connect(key_t key)
{
    if(m_shmHead != NULL)
    {
        return true;
    }

    // 如果存在则获取,不存在则返回
    int shmid = shmget((key_t)key, 0, IPC_EXCL);
    if(shmid == -1)
    {
        return false;
    }
    
    m_shmHead = shmat(shmid, 0, IPC_EXCL);
    if(m_shmHead == (void *)-1)
    {
       return false;
    }

    m_payload = (char *)((shmHead_t*)m_shmHead + 1);

    return true;
}

bool CShmFIFO::Separate()
{
    if(shmdt(m_shmHead) == -1)
    {
        return false;
    }

    m_shmHead = NULL;
    m_payload = NULL;

    return true;
}

bool CShmFIFO::Distory(key_t key)
{
    shmHead_t *pHead = (shmHead_t *)m_shmHead;
    int shmid = pHead->shmid;

    // 销毁信号量
    sem_destroy (&pHead->sem_full);
	sem_destroy (&pHead->sem_empty);
	sem_destroy (&pHead->sem_mutex);

    Separate();

    // 销毁共享内存
    if(shmctl(shmid, IPC_RMID, 0) == -1)		//删除共享内存
	{
		return false;
	}

    m_shmHead = NULL;
    m_payload = NULL;

    return true;
}

bool CShmFIFO::Read(char *buf, size_t bufLen)
{
    shmHead_t *pHead = (shmHead_t *)m_shmHead;
    sem_wait(&pHead->sem_empty);       // 不为空
    sem_wait(&pHead->sem_mutex);

    printf("---------------------------------------------\n");
    printf("%d正在读取 readIndex:%ld\n", getpid(), pHead->readIndex);
    int len = bufLen < pHead->blockSize ? bufLen : pHead->blockSize;
    memcpy(buf, m_payload + (pHead->readIndex) * (pHead->blockSize), len);
    pHead->readIndex = (pHead->readIndex + 1) % (pHead->blockNum);
    printf("%d正在读取结束 readIndex:%ld\n", getpid(), pHead->readIndex);
    printf("---------------------------------------------\n\n");

    sem_post(&pHead->sem_mutex);
    sem_post(&pHead->sem_full);

    return true;
}

bool CShmFIFO::Write(char *buf, size_t bufLen)
{
    shmHead_t *pHead = (shmHead_t *)m_shmHead;
    sem_wait(&pHead->sem_full);           // 不为满 - 1
    sem_wait(&pHead->sem_mutex);

    printf("---------------------------------------------\n");
    printf("%d正在写入 readIndex:%ld\n", getpid(), pHead->writeIndex);
    int len = bufLen < pHead->blockSize ? bufLen : pHead->blockSize;
    memset(m_payload + (pHead->writeIndex) * (pHead->blockSize), 0, pHead->blockSize);
    memcpy(m_payload + (pHead->writeIndex) * (pHead->blockSize), buf, len);
    pHead->writeIndex = (pHead->writeIndex + 1) % (pHead->blockNum);
    printf("%d正在结束写入 readIndex:%ld\n",  getpid(), pHead->writeIndex);
    printf("---------------------------------------------\n\n");

    sem_post(&pHead->sem_mutex);
    sem_post(&pHead->sem_empty);         // + 1

    return true;
}