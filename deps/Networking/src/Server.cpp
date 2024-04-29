#include "Server.h"
#include <iostream>
#include "Packet.h"
#include "Networking.h"

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

	listen_socket.SetBlocking(true);
	result = listen_socket.Listen(endpoint);
	if (result != NETWORK_SUCCESS) {
		std::cout << "Server : Failed listening... " << __FILE__ << ", line " << __LINE__ << "\n";
		return NETWORK_ERROR;
	}
	//Check for incoming data on the listen socket, meaning someone wants to connect
	WSAPOLLFD listening_socket_fd = {};
	listening_socket_fd.fd = listen_socket.GetSocket();
	listening_socket_fd.events = POLLRDNORM; //PULLRDNORM or PULLWRNORM, we're never writing to our listen socket ,
	listening_socket_fd.revents = 0; //thus we only care about wether we can read 
	master_fd.push_back(listening_socket_fd);

	std::cout << "Server : Listening on " << endpoint.ShortDesc() << "...\n";

	return NETWORK_SUCCESS;
}

int Server::Frame() {
	//TODO : Fix this manual indexing 
	use_fd = master_fd;//Keep revents clean by using a copy

	int poll = WSAPoll(use_fd.data(), use_fd.size(), 1);

	if (poll == SOCKET_ERROR) {
		std::cout << "Server : Poll error : " << WSAGetLastError() << "\n";
		return 0;
	}
	if (poll == 0) {
		return 1;
	}

	int accepted = AcceptConnections(use_fd[0]); // listening:fd
	
	std::vector<int> to_drop;

	for (int i = use_fd.size() - 1; i >= 1; i--) {//Backwards to avoid decrementing durin closing connections
		int connection_idx = i - 1;
		TCPConnection& connection = connections[connection_idx];

		SHORT rvnt = use_fd[i].revents;
		
		int errors = CheckForPollErrors(rvnt);
		if (errors != 0) {
			CloseConnection(connection_idx, std::to_string(errors) + " poll errors");
			continue;
		}

		if (use_fd[i].revents & POLLRDNORM) { // bitwise AND -> check for error on the connection

			
			int bytes_recieved = 0;

			if (connection.task == PacketTask::ProcessPacketSize) {
				//If this by chance only sends one of two bytes from uint16-> extraction offset will take care of it. 
				bytes_recieved = recv(	use_fd[i].fd, 
										(char*)&connection.packet_size + connection.extraction_offset, 
										sizeof(uint16_t) - connection.extraction_offset, 
										0);
			}else if (connection.task == PacketTask::ProcessPacketContents) {
				bytes_recieved = recv(use_fd[i].fd,
					(char*)&connection.buffer + connection.extraction_offset,
					connection.packet_size - connection.extraction_offset,
					0);
			}


			if (bytes_recieved == 0) {
				CloseConnection(connection_idx, std::to_string(errors) + "0 bytes received");
				continue;
			}
			if (bytes_recieved == SOCKET_ERROR) {
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK) {//Close connection unless blocking-error
					CloseConnection(connection_idx, std::to_string(errors) + "Wouldblock error");
					continue;
				}
			}

			if (bytes_recieved > 0) {//Data was recieved

				connection.extraction_offset += bytes_recieved;

				if (connection.task == PacketTask::ProcessPacketSize) {
					if (connection.extraction_offset == sizeof(uint16_t)) {
						connection.packet_size = ntohs(connection.packet_size);
						if (connection.packet_size > max_packet_size) {
							CloseConnection(connection_idx, "Packet size too large");
							continue;
						}

						connection.extraction_offset = 0;
						connection.task = PacketTask::ProcessPacketContents;
					}
				}else if (connection.task == PacketTask::ProcessPacketContents) {

					Packet packet;
					packet.buffer.resize(connection.packet_size);
					memcpy(&packet.buffer[0], connection.buffer, connection.packet_size);

					int processed = ProcessPacket(packet);
					if (processed != 1) {
						CloseConnection(connection_idx, "Failed to process packet");
						continue;
					}
					connection.packet_size = 0;
					connection.extraction_offset = 0;
					connection.task = PacketTask::ProcessPacketSize;
				}
			}
		}
	}

	return 1;
}


int Server::CheckForPollErrors(SHORT revent) {
	int errors = 0;
	if (revent & POLLERR) {
		errors++;
	}
	if (revent & POLLNVAL) {
		errors++;
	}
	if (revent & POLLHUP) {
		errors++;
	}
	return errors;
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

	TCPConnection accepted_connection(connected_socket, connected_endpoint);
	connections.emplace_back(accepted_connection);

	WSAPOLLFD accepted_fd = {};
	accepted_fd.fd = connected_socket.GetSocket();
	accepted_fd.events = POLLRDNORM;
	accepted_fd.revents = 0;
	master_fd.push_back(accepted_fd);

	std::cout << "Server : Accepted connection at " << accepted_connection.ToString() << " \n";
	return 1;
}

int Server::CloseConnection(int idx, std::string reason) {
	TCPConnection& connection = connections[idx];
	connection.Close();

	master_fd.erase(master_fd.begin() + idx + 1);
	use_fd.erase(use_fd.begin() + idx + 1);
	connections.erase(connections.begin() + idx);

	std::cout << "Server : Closed connection " << connection.ToString() << " : " << reason << "\n";
	return 1;

}

int Server::ProcessPacket(Packet& packet) {
	switch (packet.GetPacketType()) {
	case PacketType::Invalid:
		std::cout << "Server : Invalid Packet...\n";
		break;
	case String: {
		std::string msg;
		packet >> msg;
		std::cout << "Message : " << msg << "\n";
		break;
	}
	case IntegerArray: {
		uint32_t array_size = 0;
		packet >> array_size;
		std::cout << "Int[" << array_size << "] : ";
		for (int i = 0; i < array_size; i++) {
			uint32_t element = 0;
			packet >> element;
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

int Server::Run() {
	while (true) {
		Frame();

		Sleep(200);
	}
}