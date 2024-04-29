#pragma once

#include "Socket.h"
#include "Constants.h"

enum PacketTask {
	ProcessPacketSize,
	ProcessPacketContents,
};
struct TCPConnection {
	TCPConnection(IPSocket s, IPEndpoint ep) {
		socket = s;
		endpoint = ep;
	};
	IPSocket socket;
	IPEndpoint endpoint;

	PacketTask task = PacketTask::ProcessPacketSize;
	int extraction_offset = 0;
	uint16_t packet_size = 0;

	char buffer[max_packet_size];

	int Close() {
		return socket.Close();
	}

	std::string ToString() {
		return "[" + endpoint.ShortDesc() + "]";
	}

};

//std::string ConnectionToString(TCPConnection tcp) {
//	
//}