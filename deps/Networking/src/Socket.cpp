#include "Socket.h"
#include "Constants.h"
#include "Packet.h"

#include "spdlog/spdlog.h"

#include <iostream>

IPSocket::IPSocket()
{

}
IPSocket::IPSocket(SOCKET socket)
{
	ip_socket = socket;
}
IPSocket::~IPSocket()
{
}

int IPSocket::Create()
{
	ip_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ip_socket == INVALID_SOCKET) {
		spdlog::error( "Socket Creation Error" + WSAGetLastError());
		return NETWORK_ERROR;
	}

	int result = SetBlocking(true);
	if (!result) {
		spdlog::error( "Set Blocking Error" + WSAGetLastError());
		return NETWORK_ERROR;
	}
	result = SetSocketOption(SocketOption::TCP_NoDelay, TRUE);
	if (!result) {
		spdlog::error( "Set NoDelay Error" + WSAGetLastError());
		return NETWORK_ERROR;
	}
	return NETWORK_SUCCESS;
}

int IPSocket::Close()
{
	if (ip_socket == INVALID_SOCKET) {
		spdlog::error( "Cannot close invalid socket");
		return 0;
	}

	int result = closesocket(ip_socket);
	if (result != 0) {
		spdlog::error( "Socket close error : " + WSAGetLastError());
		return NETWORK_ERROR;
	}

	return NETWORK_SUCCESS;
}

int IPSocket::Bind(IPEndpoint endpoint) {
	sockaddr_in addr = endpoint.GetSockaddr();
	int result = bind(ip_socket, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (result != 0) {
		spdlog::error( "Bind failed : " + WSAGetLastError());
		return NETWORK_ERROR;
	}

	return NETWORK_SUCCESS;
}

int IPSocket::Listen(IPEndpoint endpoint, int backlog) {
	if (int bound = Bind(endpoint) != 1) {
		spdlog::error( "Listen failed as binding failed...");
		return NETWORK_ERROR;
	}

	int result = listen(ip_socket, backlog);
	if (result != 0) {
		spdlog::error( "Listen failed : " + WSAGetLastError());
		return NETWORK_ERROR;
	}

	return NETWORK_SUCCESS;
}

int IPSocket::Accept(IPSocket& out_socket, IPEndpoint* out_endpoint) {
	sockaddr_in addr = {};
	int len = sizeof(sockaddr_in);

	SOCKET accepted = accept(ip_socket, (sockaddr*)&addr, &len); //Returns INVALID_SOCKET 
	if (accepted == INVALID_SOCKET) {
		spdlog::error( "Accept failed : " + WSAGetLastError());
		return NETWORK_ERROR;
	}

	out_socket = IPSocket(accepted);

	if (out_endpoint != nullptr) {//If the caller needs the endpoint, supply it
		*out_endpoint = IPEndpoint((sockaddr*)&addr);
	}

	return NETWORK_SUCCESS;
}

int IPSocket::Connect(IPEndpoint endpoint) {
	sockaddr_in addr = endpoint.GetSockaddr();
	int result = connect(ip_socket, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (result != 0) {
		std::cout << "Connection failed : " << WSAGetLastError() << "\n";
		return NETWORK_ERROR;
	}

	return NETWORK_SUCCESS;
}

int IPSocket::Send(const void* data, int byteAmount, int& bytesSent) {
	
	bytesSent = send(ip_socket, (const char*)data, byteAmount, 0);
	if (bytesSent == INVALID_SOCKET) {
		spdlog::error( "Sending error : " + std::to_string(WSAGetLastError()));
		return NETWORK_ERROR;
	}
	return NETWORK_SUCCESS;
}

int IPSocket::SendAll(const void* data, int byteAmount) {
	int totalBytesSent = 0;
	while (totalBytesSent < byteAmount) {
		int bytesRemaining = byteAmount - totalBytesSent;
		int bytesSent = 0;

		char* buffer_offset = (char*)data + totalBytesSent;
		int result = Send(buffer_offset, bytesRemaining, bytesSent);
		if (result == 0) {
			return NETWORK_ERROR;
		}
		totalBytesSent += bytesSent;
	}

	return NETWORK_SUCCESS;
}

int IPSocket::Recv(void* destination, int byteAmount, int& bytesRecieved) {
	bytesRecieved = recv(ip_socket, (char*)destination, byteAmount, NULL);

	if (bytesRecieved == 0) {
		//Gracefully closed
		std::cout << "Recv : 0 bytes received = closed gracefully...\n";
		return NETWORK_SUCCESS;
	}

	if (bytesRecieved == SOCKET_ERROR) {
		std::cout << "Recieve error : " << WSAGetLastError() << "\n";
		return NETWORK_ERROR;
	}

	return NETWORK_SUCCESS;
}
int IPSocket::RecvAll(void* destination, int byteAmount) {

	int totalBytesReceived = 0;
	while (totalBytesReceived < byteAmount) {
		int bytesRemaining = byteAmount - totalBytesReceived;
		int bytesReceived = 0;

		char* buffer_offset = (char*)destination + totalBytesReceived;
		int result = Recv(buffer_offset, bytesRemaining, bytesReceived);
		if (result == NETWORK_ERROR) {
			std::cout << "RecvAll : Error \n";
			return NETWORK_ERROR;
		}
		totalBytesReceived += bytesReceived;
	}

	return NETWORK_SUCCESS;
}



int IPSocket::Send(Packet& packet) {
	uint16_t encoded_packet_size = htons(packet.buffer.size());

	int result = SendAll(&encoded_packet_size, sizeof(uint16_t));
	if (result == NETWORK_ERROR) {
		return NETWORK_ERROR;
	}

	result = SendAll(packet.buffer.data(), packet.buffer.size());
	if (result == NETWORK_ERROR) {
		return NETWORK_ERROR;
	}

	return NETWORK_SUCCESS;
}
int IPSocket::Recv(Packet& packet) {
	packet.Clear();

	uint16_t encoded_size = 0;
	int result = RecvAll(&encoded_size, sizeof(uint16_t));
	if (result == 0) {
		return 0;
	}

	uint16_t buffer_size = ntohs(encoded_size);

	if (buffer_size > MAX_PACKET_SIZE) {
		spdlog::error( "Socket : Recv buffer size exeeded max_packet : " + buffer_size);
		return NETWORK_ERROR;
	}

	packet.buffer.resize(buffer_size);
	result = RecvAll(&packet.buffer[0], buffer_size);
	if (result == 0) {
		return 0;
	}

	return NETWORK_SUCCESS;
}
int IPSocket::SetBlocking(bool blocking) {

	unsigned long no_block = 1;
	unsigned long block = 0;
	int result = ioctlsocket(ip_socket, FIONBIO, blocking ? &block : &no_block);
	if (result == SOCKET_ERROR) {
		spdlog::error( "Socket : Failed seting blocking mode : " + WSAGetLastError());
		return NETWORK_ERROR;
	}
	return NETWORK_SUCCESS;
}

int IPSocket::SetSocketOption(SocketOption opt, BOOL value) {
	int result = 0;
	switch (opt) {
	case TCP_NoDelay:
		result = setsockopt(ip_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
		break;
	default:
		break;
	}

	if (result != 0) {
		spdlog::error( "Set Socket Option failed with error : " + WSAGetLastError());
		return NETWORK_ERROR;
	}

	return NETWORK_SUCCESS;
}