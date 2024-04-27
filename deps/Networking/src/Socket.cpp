#include "Socket.h"
#include <iostream>
Socket::Socket()
{

}

Socket::~Socket()
{
}

int Socket::Create()
{
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET) {
		std::cout << "Socket Creation error : " << WSAGetLastError() << "\n";
		return 0;
	}

	return 1;
}

int Socket::Close()
{
	if (_socket == INVALID_SOCKET) {
		std::cout << "Socket is invalid, cannot close\n";
		return 0;
	}

	int result = closesocket(_socket);
	if (result != 0) {
		std::cout << "Socket close : " << WSAGetLastError() << "\n";
		return 0;
	}

	return 1;
}

int Socket::SetSocketOption(SocketOption opt, BOOL value) {
	int result = 0;
	switch (opt) {
	case TCP_NoDelay:
		result = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
		break;
	default:
		break;
	}

	if (result != 0) {
		std::cout << "Set Socket Option failed with error : " << WSAGetLastError() << "\n";
		return 0;
	}

	return 1;
}