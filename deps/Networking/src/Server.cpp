#include "Server.h"
#include "Constants.h"
#include "Packet.h"
#include "Networking.h"
#include "PacketManager.h"

#include <iostream>


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
		Logger::Log(L_ERROR, "Server : Failed to initalize... ");
		return NETWORK_ERROR;
	}

	listen_socket = IPSocket();
	result = listen_socket.Create();
	if (result == NETWORK_ERROR) {
		Logger::Log(L_ERROR, "Server : Failed to create socket...");
		return NETWORK_ERROR;
	}

	listen_socket.SetBlocking(false);
	result = listen_socket.Listen(endpoint);
	if (result != NETWORK_SUCCESS) {
		Logger::Log(L_ERROR, "Server : Failed listening... ");
		return NETWORK_ERROR;
	}

	//Check for incoming data on the listen socket, meaning someone wants to connect
	listen_fd = {};
	listen_fd.fd = listen_socket.GetSocket();
	listen_fd.events = POLLRDNORM; //PULLRDNORM or PULLWRNORM, we're never writing to our listen socket ,
	listen_fd.revents = 0; //thus we only care about wether we can read 

	Logger::Log(L_INFO, "Server : Listening on " + endpoint.ShortDesc());

	return NETWORK_SUCCESS;
}

int Server::Frame() {

	WSAPOLLFD lfd;
	std::vector<WSAPOLLFD> fds;
	int poll_amount = 0;

	int result = Poll(lfd, fds, poll_amount); //listen fd, connection fd, amount
	if (result == NETWORK_ERROR) {
		return NETWORK_ERROR;
	}

	//This currently adds a connection 
	result = AcceptConnections(lfd); // Listening

	std::map<int, bool> to_disconnect; // index, disconnect
	//For every file descriptor polled 
	for (int i = 0; i < fds.size(); i++) { // Read data in packetmanagers
		result = ServiceConnection(connections[i].tcp, fds[i]);
		if (result == NETWORK_ERROR) {
			to_disconnect.insert({ i, true });
			Logger::Log(L_DEBUG, "Server : " + connections[i].tcp.ToString() + "marked for disconnecting...");
			continue;
		}
	}

	for (int i = 0; i < connections.size(); i++) { //Disconnected connections with errors, 
		if (to_disconnect.find(i) != to_disconnect.end()) {
			connections[i].tcp.Close();
			// Remove
			OnDisconnect(connections[i].tcp, "Disconnected");
			connections.erase(connections.begin() + i);
		}
	}
	to_disconnect.clear();

	for (int i = 0; i < connections.size(); i++) { // Handle queded packets
		PacketManager& pm = connections[i].tcp.pm_read;
		while (pm.HasPendingPackets()) {
			std::shared_ptr<Packet> front = pm.Retrieve();
			int processed = ProcessPacket(front);
			if (processed == 0) {
				to_disconnect.insert({ i, true });
				break;
			}
			pm.Pop();
		}
	}

	return 1;
}

int Server::Poll(WSAPOLLFD& listening_fd, std::vector<WSAPOLLFD>& fd_vector, int& poll_amount) {
	std::vector<WSAPOLLFD> polled_fds = { listen_fd};
	for (Connection& connection : connections) {
		polled_fds.push_back(connection.fd);
	}

	poll_amount = WSAPoll(polled_fds.data(), polled_fds.size(), 1); //0th = listen, 1 to Size = connection, 

	if (poll_amount == SOCKET_ERROR) {
		Logger::Log(L_ERROR, "Server : Poll error : " + WSAGetLastError());
		return NETWORK_ERROR;
	}

	if (poll_amount == 0) { //Nothing to poll
		return NETWORK_SUCCESS;
	}

	listening_fd = polled_fds[0]; // <-- fill listen fd
	polled_fds.erase(polled_fds.begin()); //remove listen fd from polled
	fd_vector = polled_fds; // set fd_vector equal to polled

	return NETWORK_SUCCESS;
}

int Server::ServiceConnection(TCPConnection& tcp, WSAPOLLFD& fd) {
	
	PacketManager& pm_write = tcp.pm_write;
	PacketManager& pm_read = tcp.pm_read;

	if (fd.revents & POLLERR) {//Nothing to read
		Logger::Log(L_ERROR, "Server : POLLERR");
		return NETWORK_ERROR;
	}
	if (fd.revents & POLLHUP) {//Nothing to read
		Logger::Log(L_ERROR, "Server : POLLHUP");
		return NETWORK_ERROR;
	}
	if (fd.revents & POLLNVAL) {//Nothing to read
		Logger::Log(L_ERROR, "Server : POLLNVAL");
		return NETWORK_ERROR;
	}

	if (fd.revents & POLLRDNORM) {
		int bytes_recieved = Read(fd, &pm_read, &tcp);

		if (bytes_recieved == 0) {
			Logger::Log(L_ERROR, "Server : Read error, 0 bytes receieved");
			return NETWORK_ERROR;
		}
		if (bytes_recieved == WSAEWOULDBLOCK) {
			Logger::Log(L_ERROR, "Server : Read error, WOULD BLOCK");
			return NETWORK_ERROR;
		}
		if (bytes_recieved < -1) {
			Logger::Log(L_ERROR, "Server : Negative bytes received");
			return NETWORK_ERROR;
		}

		pm_read.current_packet_extraction_offset += bytes_recieved;

		if (pm_read.current_task == PacketManagerTask::ProcessPacketSize) {

			if (pm_read.current_packet_extraction_offset == sizeof(uint16_t)) {

				pm_read.current_packet_size = ntohs(pm_read.current_packet_size);

				if (pm_read.current_packet_size > max_packet_size) {
					Logger::Log(L_ERROR, "Server : Packet size exceeded MAX_PACKET_SIZE");
					return NETWORK_ERROR;
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

	return NETWORK_SUCCESS;
}

int Server::Read(WSAPOLLFD fd, PacketManager* pm, TCPConnection* tcp) { //Return amount of bytes received
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
	return NETWORK_ERROR;
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

int Server::AcceptConnections(WSAPOLLFD fd) {
	//Atleast 1 fd has the events pending we're polling for
	if (!(fd.revents & POLLRDNORM)) {//bitwise AND operation to check for flagged events 
		//Logger::Log(L_DEBUG, "Server : POLLRDNORM events");
		return 0;
	}

	IPEndpoint connected_endpoint;
	IPSocket connected_socket;

	int result = listen_socket.Accept(connected_socket, &connected_endpoint);
	if (result != NETWORK_SUCCESS) {
		Logger::Log(L_ERROR, "Server : Failed accept");
		return NETWORK_ERROR;
	}

	WSAPOLLFD accepted_fd = {}; //Create read and write fd for this connection
	accepted_fd.fd = connected_socket.GetSocket();
	accepted_fd.events = POLLRDNORM | POLLWRNORM;
	accepted_fd.revents = 0;

	Connection connection{ TCPConnection(connected_socket, connected_endpoint), accepted_fd };
	connections.push_back(connection);

	OnConnect(connections[connections.size() - 1].tcp);
	return NETWORK_SUCCESS;
}

int Server::ProcessPacket(std::shared_ptr<Packet> packet) {
	switch (packet->GetPacketType()) {
	case PacketType::Invalid:
		std::cout << "Server : Invalid Packet...\n";
		break;
	case String: {
		std::string msg;
		*packet >> msg;
		Logger::Log(L_INFO, "MESSAGE : " + msg);
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
	Logger::Log(L_INFO, "Server : " + connection.ToString() + " - New connection accepted.");
	return 1;
}

int Server::OnDisconnect(TCPConnection& connection, std::string reason) {
	Logger::Log(L_INFO, "Server : " + connection.ToString() + " - Connection lost [" + reason + "]");
	return 1;
}
