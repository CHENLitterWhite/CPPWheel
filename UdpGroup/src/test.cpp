#include "UdpGroup.h"
#include <thread>

static void* funcService(void* arg)
{
    UdpGroup udpGroup;
    udpGroup.Init(1, 2);

    udpGroup.Join("239.255.255.250", 24002);

    int num = 0;
    while (1)
    {
        char buf[1024] = { 0 };

        int len = udpGroup.getData((void*)buf, 1024);
        while (len <= 0)
        {
            len = udpGroup.getData((void*)buf, 1024);
        }
        printf("%s\n", buf);
        char recv[1024] = { 0 };
        sprintf(recv, "%d recv::2023-08-17", num + 1);

        bool ret = udpGroup.sendData(recv, strlen(recv));
        while (!ret)
        {
            ret = udpGroup.sendData(recv, strlen(recv));
        }
    }
}

static void* funcClient(void* arg)
{
    UdpGroup udpGroup;
    udpGroup.Init(1, 1);
    udpGroup.Join("239.255.255.250", 24002);

    char buf[10] = "aaaaa";

    while (1)
    {

        udpGroup.sendData(buf, sizeof(buf));

        char buf[1024] = { 0 };
        int len = udpGroup.getData((void*)buf, 1024);
        while (len <= 0)
        {
            len = udpGroup.getData((void*)buf, 1024);
            udpGroup.sendData(buf, sizeof(buf));
        }
        printf("%s\n", buf);
        Sleep(2000);
    }

    udpGroup.Free();
}

int main(void)
{

	std::thread Service(funcService, nullptr);
    std::thread Client(funcClient, nullptr);

    Service.join();
    Client.join();

	return 0;
}