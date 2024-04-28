#include "Server.h"
#include <iostream>
#include "Socket.h"
#include "Packet.h"
Server::Server()
{
}

Server::~Server()
{
}

int HandlePacket(Packet& packet) {
	switch (packet.GetPacketType()) {
	case PacketType::Invalid:
		std::cout << "Server : Invalid Packet...\n";
		break;
	case String: {
		std::string msg;
		packet >> msg;
		std::cout << "Message : " << msg << "\n";
		break;
	}	
	case IntegerArray: {
		uint32_t array_size = 0;
		packet >> array_size;
		std::cout << "Int[" << array_size << "] : ";
		for (int i = 0; i < array_size; i++) {
			uint32_t element = 0;
			packet >> element;
			std::cout << element << ", ";
		}
		std::cout << "\n";
	}
		break;
	default:
		return 0;
		break;
	}
	return 1;
}

int Server::RunThreadedServer(IPEndpoint endpoint) {
	Socket sock;
	int result = sock.Create();
	if (result != 1) {
		std::cout << "Server : Socket failed creation\n";
	}
	result = sock.Listen(endpoint, 5);
	if (result != 1) {
		std::cout << "Server : Socket failed listening\n";
	}
	std::cout << "Server : Listening to " << endpoint.ShortDesc() << "\n";

	//It is currently connection
	Socket connection;
	result = sock.Accept(connection);
	if (result != 1) {
		std::cout << "Server : No connection accepted\n";
	}
	else {
		std::cout << "Server : Connection established\n";
	}

	Packet packet;

	while (true) {
		int result = connection.Recv(packet);
		if (result == 0) {
			break;
		}
		try {
			if (!HandlePacket(packet)) {
				break;
			}
		}
		catch (PacketExecption& e) {
			std::cout << e.what() << "\n";
		}
		
	}
	sock.Close();
	connection.Close();
	return 1;
}