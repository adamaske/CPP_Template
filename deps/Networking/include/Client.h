#pragma once
struct IPEndpoint;
class Client{
private:

public:
    Client();
    ~Client();
    
    int Run(IPEndpoint endpoint);
    void Start();
};