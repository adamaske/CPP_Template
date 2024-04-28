#include "Networking.h"
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")

#include "Server.h"
#include "Client.h"
#include "Socket.h"

#include <thread>


Networking::Networking(){
    std::cout << "Networking Constructed\n";
}
Networking::~Networking() {
    std::cout << "Networking Destructed\n";
}

bool Networking::Intialize()
{
    int err = 0;

    WSADATA wsaData;
    err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0) {
        std::cout << "WSAStartup Failed : " << err << "\n";
        return 0;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {

        std::cerr << "Found no useable Winsock DLL\n";
        WSACleanup();
        return 0;
    }

    return 1;
}

void Networking::Shutdown()
{
    WSACleanup();
}

