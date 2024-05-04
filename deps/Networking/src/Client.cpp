#include "Client.h"
#include "Constants.h"
#include "Packet.h"

#include "spdlog/spdlog.h"

#include <iostream>

Client::Client() {
}

Client::~Client() {
}

int Client::Connect(IPEndpoint endpoint) {
	is_connected = false;
	ip_socket = IPSocket();

	int result = ip_socket.Create();
	if (result == NETWORK_ERROR) {
		return NETWORK_ERROR;
	}

	ip_socket.SetBlocking(true);

	result = ip_socket.Connect(endpoint);
	if (result == NETWORK_ERROR) {
		return NETWORK_ERROR;
	}

	is_connected = true;
	return NETWORK_SUCCESS;
}

bool Client::IsConnected() {
	return is_connected;
}

int Client::Frame() {

	Packet s_p(PacketType::String);
	s_p << std::string("Hello from Client!");
	int bytes = 0;
	spdlog::debug( "Client : Sending packet... ");
	int result = ip_socket.Send(s_p);
	if (result == NETWORK_ERROR) {
		spdlog::debug( "Client : Send error, disconnected");
		ip_socket.Close();
		is_connected = false;
		return 0;
	}

	return 1;
}

int Client::Run(IPEndpoint endpoint, bool* running) {
	while (running) {
		if (!is_connected) {
			Connect(endpoint);
		}


		Frame();

		Sleep(500);
	}

	return 1;
}
