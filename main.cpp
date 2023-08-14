#include "SsdpUdpGroup.h"

int main(void)
{
    SsdpUdpGroup ssdpGroup;
    ssdpGroup.InitSocket();
    ssdpGroup.Start();

    return 0;
}