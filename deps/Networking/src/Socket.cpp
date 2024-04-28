#include "Socket.h"
#include <iostream>
Socket::Socket()
{

}
Socket::Socket(SOCKET socket)
{
	ip_socket = socket;
}
Socket::~Socket()
{
}

int Socket::Create()
{
	ip_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ip_socket == INVALID_SOCKET) {
		std::cout << "Socket Creation error : " << WSAGetLastError() << "\n";
		return 0;
	}

	return 1;
}

int Socket::Close()
{
	if (ip_socket == INVALID_SOCKET) {
		std::cout << "Socket is invalid, cannot close\n";
		return 0;
	}

	int result = closesocket(ip_socket);
	if (result != 0) {
		std::cout << "Socket close : " << WSAGetLastError() << "\n";
		return 0;
	}

	return 1;
}

int Socket::Bind(IPEndpoint endpoint) {
	sockaddr_in addr = endpoint.GetSockaddr();
	int result = bind(ip_socket, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (result != 0) {
		std::cout << "Bind failed : " << WSAGetLastError() << "\n";
		return 0;
	}

	return 1;
}

int Socket::Listen(IPEndpoint endpoint, int backlog) {
	if (int bound = Bind(endpoint) != 1) {
		std::cout << "Listen failed as binding failed...\n";
		return 0;
	}

	int result = listen(ip_socket, backlog);
	if (result != 0) {
		std::cout << "Listen failed : " << WSAGetLastError() << "\n";
		return 0;
	}

	return 1;
}
int Socket::Accept(Socket& out) {
	SOCKET accepted = accept(ip_socket, nullptr, nullptr); //Blocking function
	if (accepted == INVALID_SOCKET) {
		std::cout << "Accept failed : " << WSAGetLastError() << "\n";
		return 0;
	}
	out = Socket(accepted);
	return 1;
}

int Socket::Connect(IPEndpoint endpoint) {
	sockaddr_in addr = endpoint.GetSockaddr();
	int result = connect(ip_socket, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (result != 0) {
		std::cout << "Connection failed : " << WSAGetLastError() << "\n";
		return 0;
	}

	return 1;
}

int Socket::SetSocketOption(SocketOption opt, BOOL value) {
	int result = 0;
	switch (opt) {
	case TCP_NoDelay:
		result = setsockopt(ip_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
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