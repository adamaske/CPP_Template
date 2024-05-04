#pragma once

#include "Socket.h"
#include "Constants.h"
#include "PacketManager.h"
struct TCPConnection {
	IPSocket socket;
	IPEndpoint endpoint;

	int read_buffer_size = 0; //
	char read_buffer[MAX_PACKET_SIZE] = {}; //

	int write_buffer_size = 0;
	char write_buffer[MAX_PACKET_SIZE] = {};

	//Wrappers for packet queues
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
