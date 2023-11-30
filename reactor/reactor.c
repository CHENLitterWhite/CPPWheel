/*
    支持百万并发的 reactor
    1.其主要限制在于Linux系统的限制,需要修改一些参数

    测试: 3个标准
    1.wrk       --> qps 一秒钟能处理的请求量
    2.并发连接量 --> 
    3.iperf     --> 测试带宽(硬件能力)

    // gcc reactor.c -oreactor -mcmodel=medium -g
*/

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define PORT 2480
#define MAX_EVENTS 1024

#define MAX_BUFFER_SIZE 1024

// 事件结构 -- 封装 读写缓冲区,读写事件处理方法
typedef int (*RCALLBACK)(int fd);

typedef struct _CONN_ITEM_{

    int fd;
    int rLen;
    int wLen;
    char readBuffer[MAX_BUFFER_SIZE];
    char writeBuffer[MAX_BUFFER_SIZE];

    // 函数指针 -- 这里我们根据 epoll返回的特性分为读事件和写事件
    RCALLBACK send_cb;
    RCALLBACK recv_cb;

}CONN_ITEM, LPTIEM;

CONN_ITEM connLists[1048576] = {0};
int epfd;

// 读事件
int send_cb(int fd){

    int Len = connLists[fd].wLen;
    int count = send(fd, connLists[fd].readBuffer, Len, 0);


    struct epoll_event evnet;
    evnet.data.fd = fd;
    evnet.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &evnet);

}

// 写事件
int recv_cb(int fd){

    char *buffer = connLists[fd].readBuffer;
    int rLen = connLists[fd].rLen;

    int count = recv(fd, buffer + rLen, MAX_BUFFER_SIZE - rLen, 0);
    if(0 == count){
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
    }
    else
    {
        connLists[fd].rLen += count;

        // 回写数据
        memcpy(connLists[fd].writeBuffer, connLists[fd].readBuffer, connLists[fd].rLen);
        connLists[fd].rLen -= 0;
        connLists[fd].wLen = connLists[fd].rLen;

        // 将事件修改为触发写事件
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLOUT;               
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
    }
}


// 连接事件 - 属于读事件
int accept_cb(int fd){

    int ret = 0;

    struct sockaddr_in clienAddr;
    socklen_t len = sizeof(clienAddr);
    int connfd = accept(fd, (struct sockaddr*)&clienAddr, &len);
    if(connfd){
        // 将新的 fd加入到epoll中
        struct epoll_event event;
        event.data.fd = connfd;
        event.events = EPOLLIN;
        connLists[connfd].send_cb = send_cb;
        connLists[connfd].recv_cb = recv_cb;

        connLists[connfd].rLen = 0;
        connLists[connfd].wLen = 0;
        memset(connLists[connfd].readBuffer, 0, MAX_BUFFER_SIZE);
        memset(connLists[connfd].writeBuffer, 0, MAX_BUFFER_SIZE);

        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &event);
        if(-1 == ret){
            perror("epoll_ctl");
        }
    }

}

int main(){

    int ret = 0;
    int i = 0;

    // TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd){
        perror("socket");
        return -1;
    }

    // addr
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));
    if(-1 == ret){
        perror("bind");
        return -1;
    }

    // 这个地方的第二个参数，表示未连接的个数能容纳几个，防止syn泛洪
    listen(sockfd, 10);

    // 建立 epoll,参数以无意义(内核>=2.6.8) > 0
    epfd =  epoll_create(10);
    if(-1 == ret){
        perror("epoll_create");
        return -1;
    }

    // 将sockfd加入到 epoll中
    struct epoll_event event;
    event.data.fd = sockfd;
    event.events = EPOLLIN;           // 指定监控的事件
    connLists[sockfd].fd = sockfd;
    //connLists[sockfd].accept_cb = accept_cb;
    connLists[sockfd].recv_cb = accept_cb;
    connLists[sockfd].send_cb = send_cb;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
    if(-1 == ret){
        perror("epoll_ctl");
        return -1;
    }

    // 不断的监视消息 - 可能会触发多次
    struct epoll_event events[MAX_EVENTS];
    while(1){

        int ready = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if(-1 == ret){
            perror("epoll_wait");
            return -1;
        }

        for(i = 0; i < ready; ++i){
            
            int fd = events[i].data.fd;
            if(events[i].events & EPOLLIN){
                // 读事件
                ret = connLists[fd].recv_cb(fd);
            }
            else if(events[i].events & EPOLLOUT){
                // 写事件
                ret = connLists[fd].send_cb(fd);
            }
        }
    }
    
    close(epfd);
    close(sockfd);

    return 0;
}



