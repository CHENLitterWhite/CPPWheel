/*
*    信号量实现是共享内存 
*    -- 支持多读多写
*    -- 支持跨进程
*    ipcs  -m
*    ipcrm -m <shmid>
*/

#ifdef __cplusplus
extern "C"
{
#include <cstddef>
#include <cstdint>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>

#pragma pack(push, 1)
typedef struct
{
    int      shmid;              // 共享内存ID
    uint16_t blockSize;          // 块大小
    uint16_t blockNum;           // 块数量
    uint16_t readIndex;          // 读索引
    uint16_t writeIndex;         // 写索引

    sem_t    sem_mutex;          // 竞争互斥量信号量
    sem_t    sem_full;           // 控制共享内存是否满的信号量
    sem_t    sem_empty;          // 控制共享内存是否空的信号量

}shmHead_t;
#pragma pack(pop)
};
#endif

class CShmFIFO
{
public:

    CShmFIFO();
    ~CShmFIFO();

    bool Init(key_t key, uint16_t blockSize, uint16_t blockNum);     // 初始化,调用一次
    bool Connect(key_t key);                                         // 连接到共享内存
    bool Separate();                                                 // 分离
    bool Distory(key_t key);                                         // 销毁,调用一次

    bool Read(char *buf, size_t bufLen);
    bool Write(char *buf, size_t bufLen);

private:
    void* m_shmHead;             // 头部地址
    char* m_payload;             // 数据开始位置
    bool  m_stop;
};