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

void Networking::TestNetwork() {
    int result = 0;

    Socket sock;
    result = sock.Create();
    if (result != 1) {
        std::cout << "Socket failed creation\n";
    }



    Server server;
    Client client;

    result = sock.Close();
}
int Networking::ForFun() {
    // IP Address is an address that can be used to identify a machine to interact with
    // 127.0.0.1 is the local machine
    // PORT is an address in a specfic machine
    // SOCKET is the combination of an IP Adress and PORT
       
    // A packet has the following:
    // SOURCE IP Address
    // SOURCE Port
    // DESTINATION IP Address
    // DESTINATION Port

    // Ipv4
    // 32 bit IP Address - Only 4 billion unique IP addresses
    // Example: 127.0.0.1 - localhost, 192.168.0.1 - Local IP Adress, 74.76.230.30 - External IP Address
    // 
    // IPv6
    // 128 bit IP Address - 340 undecillion IP addresses
    // Example: 2001:0D88:AC10:FE01:: -> ::1 - Localhost, 
    // 
       
    // NAT - Network Address Translation

    // Endianness & Serialization
    // Either little endian or big endian is used
    // Big endian - The most significant byte is the first byte
    // [ 0 1 ] = 1
    // Little endian - The least significant byte is the first byte
    // [ 1 0 ] = 1
    // Serialization -> Make sure the integer is in a format that can be converted to represent the same integer as sent
    // Network byte order is Big endian
    // Windwos uses Little endian, thus integer must be converted

    //TCP vs UDP
    // TCP requires a connection to be established
    // TCP is reliable, you know the status of data
    // Slower, more overhead
    // Attempt to send packet of 1200 bytes - Options (No dropped connection): 1. All 1200 bytes were sent, 2. Part of packet i.e 400 out of 1200, 800 remain
    // 
    // UDP is connectionless
    // Unreliable. You dont know if a packet was receieved or dropped
    // Faster, less overhead
    // There are no partial packets
    // Packets may arrive out of order
    // Packets may be duplicated

    return 0;
}