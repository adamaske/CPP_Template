#include "Client.h"

#include <iostream>

#include "Socket.h"
#include "Packet.h"

Client::Client() {
}

Client::~Client() {
}

int Client::Run(IPEndpoint endpoint) {
	Socket sock;
	int result = sock.Create();
	sock.Connect(endpoint);

	Packet s_p(PacketType::String);
	s_p << std::string("Hello from Client!");

	while (true) {
		int result = sock.Send(s_p);
		if (result == 0) {
			break;
		}

		std::cout << "Client : Sending data chunk...\n";
		Sleep(500);
	}

	sock.Close();
	return 1;
}

void Client::Start(){
    std::cout << "Client start\n";
}