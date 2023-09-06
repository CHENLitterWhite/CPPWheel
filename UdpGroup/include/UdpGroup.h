#pragma once
#ifdef _Win32
#include <winsock2.h>  
#include <ws2tcpip.h>
#include <stdio.h>  
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif
class UdpGroup
{
public:

    UdpGroup();
    ~UdpGroup();

    bool Init(int sec, int type);

    bool Join(const char* ip, int port);

    bool sendData(void* buf, int bufLen);
    int  getData(void* buf, int bufLen);

    void Free();

private:

    int m_type = 0;

#if defined(WIN32)
    SOCKET sendFd;         // 用于发送UDP的sock
    SOCKET recvFd;         // 用户接收UDP的sock(监听)
    SOCKET reSendFd;       // 单播回复
    WSADATA wsaData;
#else
    int sendFd;
    int recvFd;
    int reSendFd;
#endif

    sockaddr_in sAddr;
    sockaddr_in cAddr;
    sockaddr_in cliAddr;
};