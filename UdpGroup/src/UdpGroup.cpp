#include "UdpGroup.h"

UdpGroup::UdpGroup()
{

}

UdpGroup::~UdpGroup()
{

}

bool UdpGroup::Init(int sec, int type)
{
#if defined(WIN32)
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	timeval TimeOut = { 0 };
	int ret = 0;

	TimeOut.tv_sec = sec;
	TimeOut.tv_usec = 0;
	
    switch (type)
    {
    case 1:
    {
        m_type = 1;
        //--------------------------------发送-----------------------------//
        sendFd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sendFd < 0)
        {
            printf("socket: %s\n", WSAGetLastError());
            return false;
        }

        ret = setsockopt(sendFd, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(timeval));
        if (ret < 0)
        {
            printf("setsockopt: %s\n", WSAGetLastError());
            return false;
        }
        break;
    }
    case 2:
    {
        m_type = 2;
        //--------------------------------接收-----------------------------//
        recvFd = socket(AF_INET, SOCK_DGRAM, 0);
        if (recvFd < 0)
        {
            printf("socket: %s\n", WSAGetLastError());
            return false;
        }

        ret = setsockopt(recvFd, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(timeval));
        if (ret < 0)
        {
            printf("setsockopt: %s\n", WSAGetLastError());
            return false;
        }

        //------------------------------单播返回-------------------------//
        reSendFd = socket(AF_INET, SOCK_DGRAM, 0);
        if (reSendFd < 0)
        {
            printf("setsockopt: %s\n", WSAGetLastError());
            return false;
        }

        ret = setsockopt(reSendFd, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(timeval));
        if (ret < 0)
        {
            printf("setsockopt: %s\n", WSAGetLastError());
            return false;
        }
        break;
    }
    default:
        return false;
    }

    return true;

}

bool UdpGroup::Join(const char* ip, int port)
{
    int ret = 0;

    if (m_type == 1)
    {
        //--------------------------------发送-----------------------------//
        ip_mreq mreqSend;
        mreqSend.imr_multiaddr.s_addr = inet_addr(ip);
        mreqSend.imr_interface.s_addr = htonl(INADDR_ANY);
        ret = setsockopt(sendFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreqSend, sizeof(mreqSend));
        if (ret < 0)
        {
            printf("setsockopt: %s\n", WSAGetLastError());
            return false;
        }

        memset(&sAddr, 0, sizeof(sAddr));
        sAddr.sin_family = AF_INET;
        sAddr.sin_addr.s_addr = inet_addr(ip);
        sAddr.sin_port = htons(port);
    }
    else if (m_type == 2)
    {
        //--------------------------------接收-----------------------------//
        ip_mreq mreqRecv;
        memset(&mreqRecv, 0, sizeof(mreqRecv));
        mreqRecv.imr_multiaddr.s_addr = inet_addr(ip);
        mreqRecv.imr_interface.s_addr = INADDR_ANY;
        ret = setsockopt(recvFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreqRecv, sizeof(mreqRecv));
        if (ret < 0)
        {
            printf("setsockopt: %s\n", WSAGetLastError());
            return false;
        }

        memset(&cAddr, 0, sizeof(cAddr));
        cAddr.sin_family = AF_INET;
        cAddr.sin_addr.s_addr = INADDR_ANY;
        cAddr.sin_port = htons(port);

        ret = bind(recvFd, (sockaddr*)&cAddr, sizeof(cAddr));
        if (ret < 0)
        {
            printf("setsockopt: %s\n", WSAGetLastError());
            return false;
        }
    }

    return false;
}

bool UdpGroup::sendData(void* buf, int bufLen)
{
    int ret = 0;

    if (m_type == 1)
    {
        ret = sendto(sendFd, (char *)buf, bufLen, 0, (sockaddr*)&sAddr, sizeof(sAddr));
        if (ret <= 0)
        {
            return false;
        }
    }
    else if (m_type == 2)
    {
        int len = sendto(reSendFd, (char*)buf, bufLen, 0, (sockaddr*)&cliAddr, sizeof(cliAddr));
        if (len < 0)
        {
            return false;
        }
    }

    return true;
}

int UdpGroup::getData(void* buf, int bufLen)
{
    int len = 0;
    if (m_type == 1)
    {
        len = recvfrom(sendFd, (char*)buf, bufLen, 0, NULL, 0);
    }
    else if (m_type == 2)
    {
        memset(&cliAddr, 0, sizeof(cliAddr));
        socklen_t lenAddr = sizeof(cliAddr);
        len = recvfrom(recvFd, (char*)buf, bufLen, 0, (sockaddr*)&cliAddr, &lenAddr);
    }

    return len;
}

void UdpGroup::Free()
{
#if defined(WIN32)
    closesocket(sendFd);
    closesocket(recvFd);
    closesocket(reSendFd);
    WSACleanup();
#else
    close(sendFd);
    close(recvFd);
    close(reSendFd);
#endif
}