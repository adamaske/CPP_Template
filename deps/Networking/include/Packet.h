#pragma once
#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#include <vector>
#include <string>
#include "Constants.h"

enum PacketType {
	Invalid, 
	String,
	IntegerArray, 

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

class Packet {
public:

	uint32_t extraction_offset = sizeof(PacketType);
	std::vector<char> buffer;
	Packet(PacketType type = PacketType::Invalid) {
		Clear();
		AssignPacketType(type);
	};
	
	PacketType GetPacketType() {
		PacketType* packet_type_ptr = reinterpret_cast<PacketType*>(&buffer[0]);
		return static_cast<PacketType>(ntohs(*packet_type_ptr)); //Convert to host-byte-order
	}

	void AssignPacketType(PacketType packet_type) {
		PacketType* packet_type_ptr = reinterpret_cast<PacketType*>(&buffer[0]); //Look at the first 2 bytes as a packettype
		*packet_type_ptr = static_cast<PacketType>(htons(packet_type)); //Convert to network-byte-order
	}

	void Clear() {
		buffer.resize(sizeof(PacketType));
		AssignPacketType(Invalid);
		extraction_offset = sizeof(PacketType);
	}

	void Append(const void* data, uint32_t size) {
		if (buffer.size() + size > max_packet_size) {
			throw PacketExecption("[Packet::Append(const void*, uint32_t)] - Packet size exceeded max_packet.");
		}
		buffer.insert(buffer.end(), (char*)data, (char*)data + size);
	}

	Packet& operator << (uint32_t data) {
		data = htonl(data);
		Append(&data, sizeof(uint32_t));
		return *this;
	}

	Packet& operator >> (uint32_t& data) {
		if ((extraction_offset + sizeof(uint32_t)) > buffer.size()){
			throw PacketExecption("[Packet::operator >>(uint32_t &)] - Extraction offset exceeds buffer size.");
		}
		data = *reinterpret_cast<uint32_t*>(&buffer[extraction_offset]);
		data = ntohl(data);

		extraction_offset += sizeof(uint32_t);
		return *this;
	}

	Packet& operator << (const std::string& data) {
		*this << (uint32_t)data.size();
		Append(data.data(), data.size());
		return *this;
	}

	Packet& operator >> (std::string& data) {

		data.clear();

		uint32_t string_size = 0;
		*this >> string_size;

		if ((extraction_offset + string_size) > buffer.size()){
			throw PacketExecption("[Packet::operator >>(std::string &)] - Extraction offset exceeds buffer size.");
		}
		data.resize(string_size);
		data.assign(&buffer[extraction_offset], string_size);

		extraction_offset += string_size;
		return *this;
	}
};


