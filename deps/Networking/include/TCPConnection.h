#pragma once

#include "Socket.h"
#include "Constants.h"
#include "PacketManager.h"
struct TCPConnection {
	TCPConnection(IPSocket s, IPEndpoint ep) {
		socket = s;
		endpoint = ep;
	};
	IPSocket socket;
	IPEndpoint endpoint;


	PacketManager pm_read;
	PacketManager pm_write;
	char buffer[max_packet_size] = {};

	int Close() {
		return socket.Close();
	}

	std::string ToString() {
		return "[" + endpoint.ShortDesc() + "]";
	}

};
