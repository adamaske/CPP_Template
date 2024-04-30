#include "Server.h"
#include <iostream>
#include "Packet.h"
#include "Networking.h"
#include "PacketManager.h"
#define NETWORK_ERROR 0
#define NETWORK_SUCCESS 1

Server::Server()
{
}

Server::~Server()
{
}

int Server::Initialize(IPEndpoint endpoint) {
	int result = Networking::Intialize();
	if (result != 1) {
		//Something is wrong
		std::cout << "Server : Failed to initalize... " << __FILE__ << ", line " << __LINE__ << "\n";
		return NETWORK_ERROR;
	}

	listen_socket = IPSocket();
	result = listen_socket.Create();
	if (result == NETWORK_ERROR) {
		std::cout << "Server : Failed to create socket... " << __FILE__ << ", line " << __LINE__ << "\n";
		return NETWORK_ERROR;
	}

	listen_socket.SetBlocking(false);
	result = listen_socket.Listen(endpoint);
	if (result != NETWORK_SUCCESS) {
		std::cout << "Server : Failed listening... " << __FILE__ << ", line " << __LINE__ << "\n";
		return NETWORK_ERROR;
	}

	//Check for incoming data on the listen socket, meaning someone wants to connect
	listen_fd = {};
	listen_fd.fd = listen_socket.GetSocket();
	listen_fd.events = POLLRDNORM; //PULLRDNORM or PULLWRNORM, we're never writing to our listen socket ,
	listen_fd.revents = 0; //thus we only care about wether we can read 

	std::cout << "Server : Listening on " << endpoint.ShortDesc() << "...\n";

	return NETWORK_SUCCESS;
}

int Server::Frame() {
	std::vector<WSAPOLLFD> fds = { listen_fd, };
	for (auto& connection : as) {
		fds.push_back(connection.fd);
	}

	int poll = WSAPoll(fds.data(), fds.size(), 1); //

	if (poll == SOCKET_ERROR) {
		std::cout << "Server : Poll error : " << WSAGetLastError() << "\n";
		return 0;
	}
	if (poll == 0) { //Nothing to poll
		std::cout << "Server : No poll results\n";
		return 1;
	}
;
	int accepted = AcceptConnections(listen_fd); // Listening
	listen_fd.revents = 0;

	std::vector<int> to_disconnect = {};
	for (int i = 0; i < as.size(); i++) {

		TCPConnection& tcp = as[i].tcp;
		WSAPOLLFD& fd = as[i].fd;

		std::pair<int, std::string> result = ServiceConnection(tcp, fd);
		if (result.first == 0) {
			to_disconnect.push_back(i);
			std::cout << "Server : Service failed " << result.second << "\n";
			continue;
		}

		fd.revents = 0; // Clear revents
	}

	for (int i = 0; i < to_disconnect.size(); i++) {
		as[i].tcp.Close();
		// Remove
		OnDisconnect(as[i].tcp, "asd");
		as.erase(as.begin() + i);
	}

	for (int i = connections.size() - 1; i >= 0; i--) {
		PacketManager& pm = connections[i].pm_read;
		while (pm.HasPendingPackets()) {
			std::shared_ptr<Packet> front = pm.Retrieve();
			int processed = ProcessPacket(front);
			if (processed == 0) {
				CloseConnection(i, "Server : Packet parsing failed!");
				break;
			}
			pm.Pop();
		}
	}

	return 1;
}


std::pair<int, std::string> Server::ServiceConnection(TCPConnection& tcp, WSAPOLLFD& fd) {
	
	PacketManager& pm_write = tcp.pm_write;
	PacketManager& pm_read = tcp.pm_read;

	if (fd.revents & POLLERR) {//Nothing to read
		return {0, "POLLERR"};
	}
	if (fd.revents & POLLHUP) {//Nothing to read
		return { 0, "POLLHUP" };
	}
	if (fd.revents & POLLNVAL) {//Nothing to read
		return { 0, "POLLNVAL" };
	}

	if (fd.revents & POLLRDNORM) {

		int bytes_recieved = Read(fd, &pm_read, &tcp);
		if (bytes_recieved == 0) {
			return { 0, "0 bytes received" };
		}
		if (bytes_recieved == SOCKET_ERROR) {
			return { 0, "Wouldblock error" };
		}
		if (bytes_recieved < -1) {
			return { 0, "-" + std::to_string(bytes_recieved) + " bytes received" };
		}

		pm_read.current_packet_extraction_offset += bytes_recieved;

		if (pm_read.current_task == PacketManagerTask::ProcessPacketSize) {

			if (pm_read.current_packet_extraction_offset == sizeof(uint16_t)) {

				pm_read.current_packet_size = ntohs(pm_read.current_packet_size);

				if (pm_read.current_packet_size > max_packet_size) {
					return { 0, "Packet size too large" };
				}

				pm_read.current_packet_extraction_offset = 0;
				pm_read.current_task = PacketManagerTask::ProcessPacketContents;
			}
		}
		else if (pm_read.current_task == PacketManagerTask::ProcessPacketContents) {

			std::shared_ptr<Packet> packet = std::make_shared<Packet>();
			packet->buffer.resize(pm_read.current_packet_size);
			memcpy(&packet->buffer[0], tcp.buffer, pm_read.current_packet_size);

			pm_read.Append(packet);

			pm_read.current_packet_size = 0;
			pm_read.current_packet_extraction_offset = 0;
			pm_read.current_task = PacketManagerTask::ProcessPacketSize;
		}
	}
	
	if (fd.revents & POLLWRNORM) {
		int result = Write(fd, &pm_write, &tcp);

	}
	return { 1, "Success" };
}

int Server::Read(WSAPOLLFD fd, PacketManager* pm, TCPConnection* tcp) {
	if (pm->current_task == PacketManagerTask::ProcessPacketSize) {
		//If this by chance only sends one of two bytes from uint16-> extraction offset will take care of it. 
		 return recv(fd.fd,
			(char*)&pm->current_packet_size + pm->current_packet_extraction_offset,
			sizeof(uint16_t) - pm->current_packet_extraction_offset,
			0);
	}
	else if (pm->current_task == PacketManagerTask::ProcessPacketContents) {
		return recv(fd.fd,
			(char*)&tcp->buffer + pm->current_packet_extraction_offset,
			pm->current_packet_size - pm->current_packet_extraction_offset,
			0);
	}
	return 1;
}

int Server::Write(WSAPOLLFD fd, PacketManager* pm, TCPConnection* tcp) {
	while (pm->HasPendingPackets()) {
		if (pm->current_task == PacketManagerTask::ProcessPacketSize) {
			pm->current_packet_size = pm->Retrieve()->buffer.size();
			uint16_t big_endian_packet_size = htons(pm->current_packet_size);
			int bytes_sent = send(fd.fd,
				(char*)&big_endian_packet_size + pm->current_packet_extraction_offset,
				sizeof(uint16_t) - pm->current_packet_extraction_offset,
				0);
			if (bytes_sent > 0) {
				pm->current_packet_extraction_offset += bytes_sent;
			}

			if (pm->current_packet_extraction_offset == sizeof(uint16_t)) {
				pm->current_packet_extraction_offset = 0;
				pm->current_task = PacketManagerTask::ProcessPacketContents;
			}
			else {
				break; //avoid sending half because of blocking
			}

		}
		else if (pm->current_task == PacketManagerTask::ProcessPacketContents) {
			char* buffer_ptr = &pm->Retrieve()->buffer[0];
			int bytes_sent = send(fd.fd,
				(char*)buffer_ptr + pm->current_packet_extraction_offset,
				pm->current_packet_size - pm->current_packet_extraction_offset,
				0);
			if (bytes_sent > 0) {
				pm->current_packet_extraction_offset += bytes_sent;
			}

			if (pm->current_packet_extraction_offset == pm->current_packet_size) { //Sent entire packet contents
				pm->current_packet_extraction_offset = 0;
				pm->current_task = PacketManagerTask::ProcessPacketSize;
				pm->Pop();
			}
			else {
				break;
			}
		}
	}
	return 1;
}

int Server::AcceptConnections(WSAPOLLFD listening_fd) {
	//Atleast 1 fd has the events pending we're polling for
	if (!(listening_fd.revents & POLLRDNORM)) {//bitwise AND operation to check for flagged events 
		return 0;
	}

	IPEndpoint connected_endpoint;
	IPSocket connected_socket;

	int result = listen_socket.Accept(connected_socket, &connected_endpoint);
	if (result != NETWORK_SUCCESS) {
		std::cout << "Server : Failed accept...\n";
		return 0;
	}

	connections.emplace_back(TCPConnection(connected_socket, connected_endpoint));
	TCPConnection& accepted_tcp = connections[connections.size() - 1];
	OnConnect(accepted_tcp);

	WSAPOLLFD accepted_fd = {};
	accepted_fd.fd = connected_socket.GetSocket();
	accepted_fd.events = POLLRDNORM | POLLWRNORM;
	accepted_fd.revents = 0;

	Connection connection{ accepted_tcp, accepted_fd };
	as.push_back(connection);

	return 1;
}


int Server::CloseConnection(int idx, std::string reason) {
	
	TCPConnection& connection = connections[idx];
	OnDisconnect(connection, reason);

	connection.Close();

	master_fd.erase(master_fd.begin() + idx + 1);
	use_fd.erase(use_fd.begin() + idx + 1);
	connections.erase(connections.begin() + idx);

	return 1;

}

int Server::ProcessPacket(std::shared_ptr<Packet> packet) {
	switch (packet->GetPacketType()) {
	case PacketType::Invalid:
		std::cout << "Server : Invalid Packet...\n";
		break;
	case String: {
		std::string msg;
		*packet >> msg;
		std::cout << "Message : " << msg << "\n";
		break;
	}
	case IntegerArray: {
		uint32_t array_size = 0;
		*packet >> array_size;
		std::cout << "Int[" << array_size << "] : ";
		for (int i = 0; i < array_size; i++) {
			uint32_t element = 0;
			*packet >> element;
			std::cout << element << ", ";
		}
		std::cout << "\n";
	}
					 break;
	default:
		return 0;
		break;
	}
	return 1;
}

int Server::OnConnect(TCPConnection& connection) {
	std::cout << "Server : " << connection.ToString() << " - New connection accepted.\n";
	return 1;
}

int Server::OnDisconnect(TCPConnection& connection, std::string reason) {
	std::cout << "Server : " << connection.ToString() << " - Connection lost [" << reason << "]\n";
	return 1;
}

int Server::Run() {
	while (true) {
		Frame();

		Sleep(200);
	}
}