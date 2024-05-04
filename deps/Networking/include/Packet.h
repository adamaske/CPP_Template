#pragma once

#include "Constants.h"

#include <vector>
#include <string>

enum PacketType {
	Invalid, 
	String,
	IntegerArray, 
	Test,
};

class PacketExecption {
public:
	std::string execption;

	PacketExecption(const std::string exepction) {
		execption = exepction;
	}

	const char* what() {
		return execption.c_str();
	}
};

class BytePacket {

	uint32_t packet_size;
	std::vector<char> buffer;
};

class Packet {
public:

	uint32_t extraction_offset = sizeof(PacketType);
	std::vector<char> buffer;

	Packet(PacketType type = PacketType::Invalid);

	PacketType GetPacketType();

	//Populates the first bytes of the packet as the packet type
	void AssignPacketType(PacketType packet_type);

	void Clear();

	void Append(const void* data, uint32_t size);

	Packet& operator << (uint32_t data);

	Packet& operator >> (uint32_t& data);

	Packet& operator << (const std::string& data);

	Packet& operator >> (std::string& data);
};


