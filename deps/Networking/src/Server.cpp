#include "Server.h"
#include "Constants.h"
#include "Packet.h"
#include "Networking.h"
#include "PacketManager.h"

#include <iostream>

#include <spdlog/spdlog.h>

Server::Server()
{
}

Server::~Server()
{
}

int Server::Initialize(IPEndpoint endpoint) {
	listen_socket = IPSocket();
	int result = listen_socket.Create();
	if (result == NETWORK_ERROR) {
		spdlog::error( "Server : Failed to create socket...");
		return NETWORK_ERROR;
	}

	listen_socket.SetBlocking(false);
	result = listen_socket.Listen(endpoint);
	if (result != NETWORK_SUCCESS) {
		spdlog::error( "Server : Failed listening... ");
		return NETWORK_ERROR;
	}

	//Check for incoming data on the listen socket, meaning someone wants to connect
	listen_fd = {};
	listen_fd.fd = listen_socket.GetSocket();
	listen_fd.events = POLLRDNORM; //PULLRDNORM or PULLWRNORM, we're never writing to our listen socket ,
	listen_fd.revents = 0; //thus we only care about wether we can read 

	spdlog::info( "Server : " + endpoint.ShortDesc() + " - Listening.");

	return NETWORK_SUCCESS;
}

int Server::Frame() {
	use_fds = master_fds;
	auto lfd = listen_fd;
	int poll_amount = 0;
	int result = Poll(lfd, use_fds, poll_amount); //listen fd, connection fd, amount
	if (result == NETWORK_ERROR) {
		return NETWORK_ERROR;
	}

	//This currently adds a connection 
	result = AcceptConnections(lfd); // Listening

	std::map<int, bool> to_disconnect; // index, disconnect
	//For every file descriptor polled 
	for (int i = 0; i < use_fds.size(); i++) { // Read data in packetmanagers
		auto tcp = tcps[i];
		auto fd = use_fds[i];
		result = ServiceConnection(tcp, fd);
		if (result == NETWORK_ERROR) {
			to_disconnect.insert({ i, true });
			continue;
		}
	}

	for (int i = 0; i < use_fds.size(); i++) { //Disconnected connections with errors, 
		if (to_disconnect.find(i) != to_disconnect.end()) {
			auto tcp = tcps[i];
			auto fd = use_fds[i];
			tcp->Close();
			OnDisconnect(tcp, "Unknown.");

			tcps.erase(tcps.begin() + i);
			master_fds.erase(master_fds.begin() + i);
		}
	}

	for (int i = 0; i < tcps.size(); i++) {
		auto tcp = tcps[i];
		auto pm = &tcp->pm_read;

		if (pm->HasPendingPackets()) {
			auto packet = pm->Retrieve();

			ProcessPacket(packet);

			pm->Pop();
		}
	}
	return NETWORK_SUCCESS;
}

int Server::Poll(WSAPOLLFD& listening_fd, std::vector<WSAPOLLFD>& fd_vector, int& poll_amount) {
	std::vector<WSAPOLLFD> polled_fds = { listening_fd };
	for (auto fd : fd_vector) {
		polled_fds.push_back(fd);
	}
	
	poll_amount = WSAPoll(polled_fds.data(), polled_fds.size(), 1); //0th = listen, 1 to Size = connection, 

	if (poll_amount == SOCKET_ERROR) {
		spdlog::error( "Server : Poll error : " + std::to_string(WSAGetLastError()));
		return NETWORK_ERROR;
	}

	if (poll_amount == 0) {
		return NETWORK_SUCCESS;
	}

	listening_fd = polled_fds[0]; 

	polled_fds.erase(polled_fds.begin()); 
	fd_vector = polled_fds;

	return NETWORK_SUCCESS;
}

int Server::AcceptConnections(WSAPOLLFD fd) {
	//Atleast 1 fd has the events pending we're polling for
	if (!(fd.revents & POLLRDNORM)) {//bitwise AND operation to check for flagged events 
		//spdlog::debug( "Server : POLLRDNORM events");
		return 0;
	}

	IPEndpoint connected_endpoint;
	IPSocket connected_socket;

	int result = listen_socket.Accept(connected_socket, &connected_endpoint);
	if (result != NETWORK_SUCCESS) {
		spdlog::error("Server : Failed accept");
		return NETWORK_ERROR;
	}

	WSAPOLLFD accepted_fd = {}; //Create read and write fd for this connection
	accepted_fd.fd = connected_socket.GetSocket();
	accepted_fd.events = POLLRDNORM | POLLWRNORM;
	accepted_fd.revents = 0;

	master_fds.push_back(accepted_fd);
	std::shared_ptr<TCPConnection> tcp = std::make_shared<TCPConnection>(connected_socket, connected_endpoint);
	tcps.push_back(tcp);

	std::shared_ptr<Packet> welcome = std::make_shared<Packet>(String);
	*welcome << std::string("Welcome to server connection.");

	tcp->pm_write.Append(welcome);

	OnConnect(tcp);   
	return NETWORK_SUCCESS;
}

int Server::ServiceConnection(std::shared_ptr<TCPConnection> tcp, WSAPOLLFD& fd) {
	if (fd.revents & POLLERR) {//Nothing to read
		spdlog::error("Server : POLLERR");
		return NETWORK_ERROR;
	}
	if (fd.revents & POLLHUP) {//Nothing to read
		spdlog::error("Server : POLLHUP");
		return NETWORK_ERROR;
	}
	if (fd.revents & POLLNVAL) {//Nothing to read
		spdlog::error("Server : POLLNVAL");
		return NETWORK_ERROR;
	}

	if (fd.revents & POLLRDNORM) { // Is there anything to read on this socket ? 
		int result = Read(tcp, fd);
		if (result == NETWORK_ERROR) {
			return NETWORK_ERROR;
		}
	}
	
	if (fd.revents & POLLWRNORM) { // Is there anything to write on this socket?
		int result = Write(tcp, fd); 
		if (result == NETWORK_ERROR) {
			return NETWORK_ERROR;
		}
	}

	return NETWORK_SUCCESS;
}

int Server::Read(std::shared_ptr<TCPConnection> tcp, WSAPOLLFD& fd) {

	if (!use_packets) {
		int result = tcp->socket.Recv(&tcp->buffer, MAX_PACKET_SIZE, tcp->buffer_size);
		if (result == 0) {
			spdlog::info("Server : " + tcp->ToString() + " - Connection closed.");
			return NETWORK_ERROR;
		}
		if (result == NETWORK_ERROR) {
			spdlog::error("Recv result network error");
			return NETWORK_ERROR;
		}

		spdlog::debug("Server : " + tcp->ToString() + " - Recieved " + std::to_string(tcp->buffer_size) + " bytes");
		return NETWORK_SUCCESS;
	}

	int bytes_recieved = 0;
	PacketManager* pm = &tcp->pm_read;
	
	if (pm->current_task == ProcessPacketSize) { 
		// When reading the packet size we fill the packet size variable, 
		// the amount of bytes is size of that variable
		// Because of the 2 (or 4) bytes needed to read the packet size, 
		// we need to account for the edge case on not receieve all bytes at once,
		// include the extraction offset

		bytes_recieved = recv(	fd.fd, 
								(char*)&pm->current_packet_size + pm->current_extraction_offset, 
								sizeof(uint32_t) - pm->current_extraction_offset, 0);
	}
	else if (pm->current_task == ProcessPacketContents) {
		// Reading packet contents we want to fill the connection buffer
		// However the size of the packet can be any size,
		// so if we've already read 6 bytes, then we want to start the fill at byte 7 [6]
		// and the size of the remaining packet is packet_size - 6 bytes
		bytes_recieved = recv(	fd.fd, 
								(char*)&tcp->buffer + pm->current_extraction_offset, 
								pm->current_packet_size - pm->current_extraction_offset, 0);
	}
	spdlog::debug("Server : " + tcp->ToString() + " - " + std::to_string(bytes_recieved) + " bytes recieved");

	if (bytes_recieved == 0) { //Closed connection
		//spdlog::info("Server : " + tcp.ToString() + " - Connection was closed.");
		return NETWORK_ERROR;
	}

	if (bytes_recieved == SOCKET_ERROR) {
		//If the error is would block, we can ignore it
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return NETWORK_SUCCESS;
		}
		return NETWORK_ERROR;
	}

	//We can add on the extraction offset
	pm->current_extraction_offset += bytes_recieved;

	//Have we exceed the size of packet sizes
	if (pm->current_task == ProcessPacketSize) {
		// If we've recieved the correct amount of bytes for the integer descirbing the packet size,
		// we can intepert the results, converting to host byte order
		// check if the packet size is abvoe the maximum
		if (pm->current_extraction_offset != sizeof(uint32_t)) {
			return bytes_recieved;
		}

		//Convert network-byte-order to host-byte-order
		pm->current_packet_size = ntohl(pm->current_packet_size);

		if (pm->current_packet_size > MAX_PACKET_SIZE) { // Check packet size
			spdlog::error("Server :" + tcp->ToString() + " - Packet size exceeded MAX_PACKET_SIZE " + std::to_string(pm->current_packet_size));
			return NETWORK_ERROR;
		}
		// The packet size was successfully read, 
		// Reset and proceed with content processing
		pm->current_extraction_offset = 0;
		pm->current_task = ProcessPacketContents;

		return NETWORK_SUCCESS;
	}
	else if (pm->current_task == ProcessPacketContents) {

		// If we havent gotten all the bytes we can make a packet,
		// else continue reading until we get all bytes
		if (pm->current_extraction_offset != pm->current_packet_size) {
			return NETWORK_SUCCESS;
		}

		std::shared_ptr<Packet> packet = std::make_shared<Packet>();

		// The tcp connection buffer currently holds all the packet content data,
		// So we want to fill the new packet' buffer with those contents
		
		packet->buffer.resize(pm->current_packet_size);
		memcpy(&packet->buffer[0], tcp->buffer, pm->current_packet_size);

		//The packet is complete, add it to the queue
		pm->Append(packet);

		//Reset 
		pm->current_packet_size = 0;
		pm->current_extraction_offset = 0;
		pm->current_task = ProcessPacketSize;

		return NETWORK_SUCCESS;
	}

	return NETWORK_SUCCESS;
}

int Server::Write(std::shared_ptr<TCPConnection> tcp, WSAPOLLFD fd) {
	if (!use_packets) {
		int bytes_sent = 0;
		int result = tcp->socket.Send(&tcp->buffer, tcp->buffer_size, bytes_sent);

		if (result == NETWORK_ERROR) {
			spdlog::error("Server : Sending error : " + std::to_string(WSAGetLastError()));
			return NETWORK_ERROR;
		}

		spdlog::debug("Server : " + tcp->ToString() + " - Sent " + std::to_string(bytes_sent) + " bytes");
		return NETWORK_SUCCESS;
	}

	int bytes_sent = 0;
	PacketManager* pm = &tcp->pm_write;

	if (!pm->HasPendingPackets()) { //Dont have any queued packets
		return NETWORK_SUCCESS;
	}
	auto packet = pm->Retrieve();
	if (pm->current_task == ProcessPacketSize) {
		

		//First we must send the packet size 
		pm->current_packet_size = packet->buffer.size();
		uint16_t network_order_packet_size = htons(pm->current_packet_size);
		//Account for the possiblity of not sending all bytes at once with the extraction offset
		bytes_sent = send(	fd.fd,
							(char*)&network_order_packet_size + pm->current_extraction_offset,
							sizeof(uint16_t) - pm->current_extraction_offset, 
							0);
		
		if (bytes_sent == SOCKET_ERROR) {
			spdlog::error("Server : " + tcp->ToString() + " - Sending packet size " + std::to_string(pm->current_packet_size) + " error : " + std::to_string(WSAGetLastError()));
			return NETWORK_ERROR;
		}

		if (bytes_sent == 0) { //Likely blocking
			return NETWORK_SUCCESS;
		}

		spdlog::debug("Server : " + tcp->ToString() + " - Sent " + std::to_string(bytes_sent) + " bytes");
		pm->current_extraction_offset += bytes_sent;

		//if we  sent all the packet_size bytes, then continue to sending the contents
		if (pm->current_extraction_offset != sizeof(uint16_t)) {
			return NETWORK_SUCCESS;
		}
		pm->current_extraction_offset = 0;
		pm->current_task = ProcessPacketContents;

		return NETWORK_SUCCESS;
	}
	else if (pm->current_task == ProcessPacketContents) {
		char* buffer = &packet->buffer[0]; //Address to the first buffer element

		bytes_sent = send(fd.fd,
			(char*)buffer + pm->current_extraction_offset,
			pm->current_packet_size - pm->current_extraction_offset,
			0);

		if (bytes_sent == SOCKET_ERROR) {
			spdlog::error("Server : " + tcp->ToString() + " - Sending packet contents error : " + std::to_string(WSAGetLastError()));
			return NETWORK_ERROR;
		}

		if (bytes_sent == 0) { //Likely blocking
			return NETWORK_SUCCESS;
		}

		spdlog::debug("Server : " + tcp->ToString() + " - Sent " + std::to_string(bytes_sent) + " bytes");
		pm->current_extraction_offset += bytes_sent;
		//Did we send all the bytes of the packet contents ? 
		if (pm->current_extraction_offset != pm->current_packet_size) {
			return NETWORK_SUCCESS;
		}

		//Reset to process the next packet, size first
		pm->current_extraction_offset = 0;
		pm->current_task = ProcessPacketSize;
		pm->Pop(); // Discard the current packet

	}
}

int Server::ProcessPacket(std::shared_ptr<Packet> packet) {

	switch (packet->GetPacketType()) {

	case PacketType::Invalid:
		spdlog::error("Server : INVALID PACKET");
		break;
	case String:
	{
		std::string msg = {};
		*packet >> msg;
		spdlog::error("STRING PACKET : " + msg);
		break;
		}
	case IntegerArray: 
	{
		spdlog::info("INTETGER_ARRAY PACKET : ");
		uint32_t array_size = 0;
		*packet >> array_size;
		std::cout << "Int[" << array_size << "] : ";
		for (int i = 0; i < array_size; i++) {
			uint32_t element = 0;
			*packet >> element;
			std::cout << element << ", ";
		}
		std::cout << "\n";
		break;
	}
	case Test:
		spdlog::info("Server : TEST PACKET");
		break;
	default:
		spdlog::error("Server : DEFUALT PACKET");

		return NETWORK_ERROR;
		break;
	}
	return NETWORK_SUCCESS;
}

int Server::OnConnect(std::shared_ptr<TCPConnection> connection) {
	spdlog::info("Server : " + connection->ToString() + " - New connection accepted.");
	return 1;
}

int Server::OnDisconnect(std::shared_ptr<TCPConnection> connection, std::string reason) {
	spdlog::info( "Server : " + connection->ToString() + " - Disconnected.");
	return 1;
}

