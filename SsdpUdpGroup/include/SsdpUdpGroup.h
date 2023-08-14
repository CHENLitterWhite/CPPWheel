#pragma once
#include <iostream>

class SsdpUdpGroup
{
public:
    SsdpUdpGroup();
    ~SsdpUdpGroup();
    void InitSocket();
    void Start();
    void Stop();
    void Send(const char* data, int len);
private:
    int socket;
};