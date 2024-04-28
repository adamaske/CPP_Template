#pragma once
#define WIN32_LEAN_AND_MEAN
#include "WinSock2.h"
#include "Endpoint.h"

enum SocketOption{ TCP_NoDelay };
class Socket {
public:
	Socket();
	Socket(SOCKET sock);
	~Socket();

	int Create();
	int Close();

	int Bind(IPEndpoint endpoint);
	int Listen(IPEndpoint endpoint, int backlog = 5);
	int Accept(Socket& out);

	int Connect(IPEndpoint endpoint);

	int SetSocketOption(SocketOption opt, BOOL value);
private:
	SOCKET ip_socket = INVALID_SOCKET;
};