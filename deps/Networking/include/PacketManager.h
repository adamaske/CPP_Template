#pragma once
#include <queue>
#include <memory>
#include "Packet.h"

enum PacketManagerTask {
	ProcessPacketSize,
	ProcessPacketContents,
};

class PacketManager {
private:
	std::queue<std::shared_ptr<Packet>> packets;
public:
	PacketManager();

	void Clear();

	bool HasPendingPackets();

	void Append(std::shared_ptr<Packet> packet);

	std::shared_ptr<Packet> Retrieve();

	void Pop();

	uint16_t current_packet_size = 0;
	int current_packet_extraction_offset = 0;
	PacketManagerTask current_task = PacketManagerTask::ProcessPacketSize;
};