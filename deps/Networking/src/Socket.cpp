#include "Socket.h"
#include <iostream>
#include "Packet.h"
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
	sockaddr_in addr = {};
	int len = sizeof(sockaddr_in);
	SOCKET accepted = accept(ip_socket, (sockaddr*)&addr, &len); //Blocking function
	if (accepted == INVALID_SOCKET) {
		std::cout << "Accept failed : " << WSAGetLastError() << "\n";
		return 0;
	}
	IPEndpoint connectedEndpoint = IPEndpoint((sockaddr*)&addr);
	std::cout << "Connected to endpoint : \n";
	connectedEndpoint.PrintEndpoint();
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

int Socket::Send(const void* data, int byteAmount, int& bytesSent) {
	
	bytesSent = send(ip_socket, (const char*)data, byteAmount, 0);
	if (bytesSent == INVALID_SOCKET) {
		std::cout << "Sending error : " << WSAGetLastError() << "\n";
		return 0;
	}
	return 1;
}

int Socket::SendAll(const void* data, int byteAmount) {
	int totalBytesSent = 0;
	while (totalBytesSent < byteAmount) {
		int bytesRemaining = byteAmount - totalBytesSent;
		int bytesSent = 0;

		char* buffer_offset = (char*)data + totalBytesSent;
		int result = Send(buffer_offset, bytesRemaining, bytesSent);
		if (result == 0) {
			return 0;
		}
		totalBytesSent += bytesSent;
	}

	return 1;
}

int Socket::Recv(void* destination, int byteAmount, int& bytesRecieved) {
	bytesRecieved = recv(ip_socket, (char*)destination, byteAmount, NULL);

	if (bytesRecieved == 0) {
		//Gracefully closed
		std::cout << "Recv : 0 bytes received = closed gracefully...\n";
		return -1;
	}

	if (bytesRecieved == SOCKET_ERROR) {
		std::cout << "Recieve error : " << WSAGetLastError() << "\n";
		return 0;
	}

	return 1;
}
int Socket::RecvAll(void* destination, int byteAmount) {

	int totalBytesReceived = 0;
	while (totalBytesReceived < byteAmount) {
		int bytesRemaining = byteAmount - totalBytesReceived;
		int bytesReceived = 0;

		char* buffer_offset = (char*)destination + totalBytesReceived;
		int result = Recv(buffer_offset, bytesRemaining, bytesReceived);
		if (result == -1) {
			return -1;
		}
		if (result != 1) {
			std::cout << "RecvAll : Error \n";
			return 0;
		}
		totalBytesReceived += bytesReceived;
	}

	return 1;
}



int Socket::Send(Packet& packet) {
	uint16_t encoded_packet_size = htons(packet.buffer.size());

	int result = SendAll(&encoded_packet_size, sizeof(uint16_t));
	if (result == 0) {
		return 0;
	}

	result = SendAll(packet.buffer.data(), packet.buffer.size());
	if (result == 0) {
		return 0;
	}

	return 1;
}
int Socket::Recv(Packet& packet) {
	packet.Clear();

	uint16_t encoded_size = 0;
	int result = RecvAll(&encoded_size, sizeof(uint16_t));
	if (result == 0) {
		return 0;
	}

	uint16_t buffer_size = ntohs(encoded_size);

	if (buffer_size > max_packet) {
		std::cout << "Socket : Recv buffer size exeeded max_packet : " << buffer_size << "\n";
		return 0;
	}

	packet.buffer.resize(buffer_size);
	result = RecvAll(&packet.buffer[0], buffer_size);
	if (result == 0) {
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