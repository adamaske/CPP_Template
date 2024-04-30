#include "Client.h"
#include <iostream>

#include "Packet.h"

Client::Client() {
}

Client::~Client() {
}

int Client::Connect(IPEndpoint endpoint) {
	is_connected = false;

	ip_socket = IPSocket();
	int result = ip_socket.Create();
	if (result != 1) {
		return 0;
	}
	ip_socket.SetBlocking(true);
	result = ip_socket.Connect(endpoint);
	if (result != 1) {
		return 0;
	}
	is_connected = true;

	return 1;
}

bool Client::IsConnected() {
	return is_connected;
}

int Client::Frame() {

	Packet s_p(PacketType::String);
	s_p << std::string("Hello from Client!");

	Packet t_p(PacketType::Test);
	
	std::cout << "Client : Sending data chunk...\n";
	int result = ip_socket.Send(s_p);
	if (result == 0) {
		is_connected = false;
		return 0;
	}

	return 1;
}

int Client::Run(IPEndpoint endpoint, bool* running) {

	int result = Connect(endpoint);

	while (true) {
		if (*running) {
			result = Frame();
			Sleep(500);
		}
	}

	return 1;
}

void Client::Start(){
    std::cout << "Client start\n";
}