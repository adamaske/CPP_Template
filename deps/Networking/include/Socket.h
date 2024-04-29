#pragma once
#define WIN32_LEAN_AND_MEAN
#include "WinSock2.h"
#include "Endpoint.h"

class Packet;

enum SocketOption{ TCP_NoDelay };
class IPSocket {
public:
	IPSocket();
	IPSocket(SOCKET sock);
	~IPSocket();

	int Create();
	int Close();

	int Bind(IPEndpoint endpoint);
	int Listen(IPEndpoint endpoint, int backlog = 5);
	int Accept(IPSocket& out_socket, IPEndpoint* out_endpoint = nullptr);

	int Connect(IPEndpoint endpoint);

	int Send(const void* data, int byteAmount, int& bytesSent);
	int SendAll(const void* data, int byteAmount);

	int Recv(void* destination, int byteAmount, int& bytesRecieved);
	int RecvAll(void* destination, int byteAmount);

	int Send(Packet& packet);
	int Recv(Packet& packet);

	int SetBlocking(bool blocking);
	int SetSocketOption(SocketOption opt, BOOL value);

	SOCKET GetSocket() {
		return ip_socket;
	}
private:
	SOCKET ip_socket = INVALID_SOCKET;
};