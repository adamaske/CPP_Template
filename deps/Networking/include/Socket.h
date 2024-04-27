#pragma once
#define WIN32_LEAN_AND_MEAN
#include "WinSock2.h"

enum SocketOption{ TCP_NoDelay };
class Socket {
public:
	Socket();
	~Socket();

	int Create();
	int Close();

	int SetSocketOption(SocketOption opt, BOOL value);
private:
	SOCKET _socket = INVALID_SOCKET;
};