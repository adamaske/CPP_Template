#include "PacketManager.h"
#include "Constants.h"


PacketManager::PacketManager()
{

}

void PacketManager::Clear()
{
	packets = std::queue<std::shared_ptr<Packet>>{};
}

bool PacketManager::HasPendingPackets()
{
	return !packets.empty();
}

void PacketManager::Append(std::shared_ptr<Packet> packet)
{
	packets.push(std::move(packet));
}

std::shared_ptr<Packet> PacketManager::Retrieve()
{
	return packets.front();
}

void PacketManager::Pop()
{
	packets.pop();
}

