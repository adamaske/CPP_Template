#pragma once

#include "Socket.h"
#include "Constants.h"
#include "PacketManager.h"
struct TCPConnection {
	IPSocket socket;
	IPEndpoint endpoint;

	int buffer_size = 0; 
	char buffer[MAX_PACKET_SIZE] = {}; 

	PacketManager pm_read; 
	PacketManager pm_write;

	TCPConnection(IPSocket s, IPEndpoint ep) {
		socket = s;
		endpoint = ep;
	};

	int Close() {
		return socket.Close();
	}

	std::string ToString() {
		return endpoint.ShortDesc();
	}

};
