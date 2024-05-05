#include "Packet.h"
#define WIN32_LEAN_AND_MEAN
#include <winsock.h>

#include "spdlog/spdlog.h"
Packet::Packet(PacketType type) {
	Clear(); //Clear everything, set invalid, set extraction offset
	AssignPacketType(type); //set type
};

PacketType Packet::GetPacketType() {
	PacketType* packet_type_ptr = reinterpret_cast<PacketType*>(&buffer[0]);
	return static_cast<PacketType>(ntohl(*packet_type_ptr)); //Convert to host-byte-order
}

void Packet::AssignPacketType(PacketType packet_type) {
	PacketType* packet_type_ptr = reinterpret_cast<PacketType*>(&buffer[0]);  //Look at the first 4 bytes as a packettype
	*packet_type_ptr = static_cast<PacketType>(htonl(packet_type)); //Value of the pointer = PacketConvert to network-byte-order
}

void Packet::Clear() {
	buffer.resize(sizeof(PacketType)); // 4 bytes
	AssignPacketType(Invalid);
	extraction_offset = sizeof(PacketType); // we dont want to extract from here
}

void Packet::Append(const void* data, uint32_t size) {
	if (buffer.size() + size > MAX_PACKET_SIZE) {
		throw PacketExecption("[Packet::Append(const void*, uint32_t)] - Packet size exceeded max_packet.");
	}
	buffer.insert(buffer.end(), (char*)data, (char*)data + size);
}

Packet& Packet::operator << (uint32_t data) {
	data = htonl(data);
	Append(&data, sizeof(uint32_t));
	return *this;
}

Packet& Packet::operator >> (uint32_t& data) {
	if ((extraction_offset + sizeof(uint32_t)) > buffer.size()) {
		throw PacketExecption("[Packet::operator >>(uint32_t &)] - Extraction offset exceeds buffer size.");
	}
	data = *reinterpret_cast<uint32_t*>(&buffer[extraction_offset]);// derefrence Look at buffer[e_o] as a uint32_t-poiner
	data = ntohl(data); //this 

	extraction_offset += sizeof(uint32_t); //4 + 4 = 8 -> 
	return *this;
}

Packet& Packet::operator << (const std::string& data) {
	*this << (uint32_t)data.size();
	Append(data.data(), data.size());
	return *this;
}

Packet& Packet::operator >> (std::string& data) {

	data.clear();

	uint32_t string_size = 0;
	*this >> string_size;

	if ((extraction_offset + string_size) > buffer.size()) {
		throw PacketExecption("[Packet::operator >>(std::string &)] - Extraction offset exceeds buffer size.");
	}
	data.resize(string_size);
	data.assign(&buffer[extraction_offset], string_size);

	extraction_offset += string_size;
	return *this;
}