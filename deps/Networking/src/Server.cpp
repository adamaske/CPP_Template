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
	int result = Networking::Intialize();
	if (result != 1) {
		//Something is wrong
		spdlog::error( "Server : Failed to initalize... ");
		return NETWORK_ERROR;
	}

	listen_socket = IPSocket();
	result = listen_socket.Create();
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

	spdlog::info( "Server : " + endpoint.ShortDesc() + " - Listening on ");

	return NETWORK_SUCCESS;
}

int Server::Frame() {
	use_fds = master_fds;

	int poll_amount = 0;
	int result = Poll(listen_fd, use_fds, poll_amount); //listen fd, connection fd, amount
	if (result == NETWORK_ERROR) {
		return NETWORK_ERROR;
	}

	//This currently adds a connection 
	result = AcceptConnections(listen_fd); // Listening
	listen_fd.revents = 0;

	std::map<int, bool> to_disconnect; // index, disconnect
	//For every file descriptor polled 
	for (int i = 0; i < use_fds.size(); i++) { // Read data in packetmanagers
		result = ServiceConnection(tcps[i], use_fds[i]);
		if (result == NETWORK_ERROR) {
			to_disconnect.insert({ i, true });
			spdlog::debug( "Server : " + tcps[i].ToString() + "will disconnect");
			continue;
		}
	}

	for (int i = 0; i < use_fds.size(); i++) { //Disconnected connections with errors, 
		if (to_disconnect.find(i) != to_disconnect.end()) {
			tcps[i].Close();
			// Remove
			OnDisconnect(tcps[i], "Disconnected");
			tcps.erase(tcps.begin() + i);
		}
	}
	to_disconnect.clear();
	

	for (int i = 0; i < use_fds.size(); i++) {
		auto& tcp = tcps[i];
		auto& fd = use_fds[i];
		char* buffer = tcp.read_buffer;
		int buffer_size = tcp.read_buffer_size;

		if (buffer_size == 0) {
			continue;
		}
		
		std::string buffer_string;
		buffer_string.resize(buffer_size);
		for (int i = 0; i < buffer_size; i++) {
			buffer_string[i] = buffer[i];
		}
		
		spdlog::info("Server : " + buffer_string);

		*tcp.read_buffer = {};
		tcp.read_buffer_size = 0;
	}

	//for (int i = 0; i < connections.size(); i++) { // Handle queded packets
	//	PacketManager& pm = connections[i].tcp.pm_read;
	//	while (pm.HasPendingPackets()) {
	//		std::shared_ptr<Packet> front = pm.Retrieve();
	//		int processed = ProcessPacket(front);
	//		if (processed == 0) {
	//			to_disconnect.insert({ i, true });
	//			break;
	//		}
	//		pm.Pop();
	//	}
	//}

	return 1;
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

int Server::ServiceConnection(TCPConnection& tcp, WSAPOLLFD& fd) {
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
		int bytes_recieved = Read(tcp, fd);

		if (bytes_recieved == 0) { //Had read info but no bytes received
			spdlog::error("Server : 0 bytes receiveed : Connection closed");
			return NETWORK_ERROR;
		}
		if (bytes_recieved == WSAEWOULDBLOCK) { //This is wrong
			spdlog::error("Server : Read error, WOULD BLOCK");
			return NETWORK_ERROR;
		}
	}
	
	if (fd.revents & POLLWRNORM) { // Is there anything to write on this socket?
		int result = Write(tcp, fd); 

		if (result == NETWORK_ERROR) {
			spdlog::error("Server : Write Error " + std::to_string(WSAGetLastError()));
			return NETWORK_ERROR;
		}
	}

	return NETWORK_SUCCESS;
}

int Server::Read(TCPConnection& tcp, WSAPOLLFD& fd) {
	
	int bytes_recieved = 0;
	int result = tcp.socket.Recv(&tcp.read_buffer, max_packet_size, tcp.read_buffer_size);
	
	if (result == NETWORK_ERROR) {
		spdlog::error( "Recv result network error");
		return NETWORK_ERROR;
	}

	spdlog::debug("Server : " + tcp.ToString() + " - Recieved " + std::to_string(bytes_recieved) + " bytes");
	return NETWORK_SUCCESS;
}

int Server::Write(TCPConnection& tcp, WSAPOLLFD fd) {
	int bytes_sent = 0;
	int result = tcp.socket.Send(&tcp.write_buffer, tcp.write_buffer_size, bytes_sent);

	if (result == NETWORK_ERROR) {
		spdlog::error("Server : Sending error : " + std::to_string(WSAGetLastError()));
		return NETWORK_ERROR;
	}

	spdlog::debug("Server : " + tcp.ToString() + " - Sent " + std::to_string(bytes_sent) + " bytes");
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
		spdlog::error( "Server : Failed accept");
		return NETWORK_ERROR;
	}

	WSAPOLLFD accepted_fd = {}; //Create read and write fd for this connection
	accepted_fd.fd = connected_socket.GetSocket();
	accepted_fd.events = POLLRDNORM | POLLWRNORM;
	accepted_fd.revents = 0;

	master_fds.push_back(accepted_fd);
	tcps.push_back(TCPConnection(connected_socket, connected_endpoint));

	OnConnect(tcps[tcps.size()-1]);
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
		spdlog::info( "MESSAGE : " + msg);
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
	spdlog::info("Server : " + connection.ToString() + " - New connection accepted");
	return 1;
}

int Server::OnDisconnect(TCPConnection& connection, std::string reason) {
	spdlog::info( "Server : Disconnected " + connection.ToString() + " - [" + reason + "]");
	return 1;
}
