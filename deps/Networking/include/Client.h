#pragma once
#include "Socket.h"
struct IPEndpoint;
class Client{
private:

public:
    Client();
    ~Client();
    
    int Connect(IPEndpoint endpoint);
    bool IsConnected();
    int Frame();

    int Run(IPEndpoint endpoint, bool* running);
    void Start();

private:
    IPSocket ip_socket;
    bool is_connected = false;
};