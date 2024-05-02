#pragma once

#include "Socket.h"
#include "Constants.h"
#include "PacketManager.h"
struct TCPConnection {
	IPSocket socket;
	IPEndpoint endpoint;

	int read_buffer_size = 0; //
	char read_buffer[max_packet_size] = {}; //

	int write_buffer_size = 0;
	char write_buffer[max_packet_size] = {};

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
