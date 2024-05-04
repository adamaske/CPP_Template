#pragma once
#include "Socket.h"
struct IPEndpoint;
class Client{
private:

public:
    Client();
    ~Client();
    
    int Connect(IPEndpoint endpoint);
    int Frame();

    int Run(IPEndpoint endpoint, bool* running);

    bool IsConnected();
private:
    IPSocket ip_socket;
    bool is_connected = false;
};