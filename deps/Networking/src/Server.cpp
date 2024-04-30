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

	listen_socket.SetBlocking(true);
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

	//How does this work ?
	PollListener();
	PollConnections();


	//TODO : This is such a horrible function i am loosing my mind
	use_fd = master_fd;//Keep revents clean by using a copy
	use_fd.emplace_back(listen_fd);

	std::vector<WSAPOLLFD> fds = { listen_fd, };
	for (auto& connection : as) {
		fds.push_back(connection.fd);
	}

	int poll = WSAPoll(fds.data(), fds.size(), 1); //POLL
	int poll = WSAPoll(use_fd.data(), use_fd.size(), 1);

	if (poll == SOCKET_ERROR) {
		std::cout << "Server : Poll error : " << WSAGetLastError() << "\n";
		return 0;
	}
	if (poll == 0) { //Nothing to poll
		return 1;
	}

	WSAPOLLFD lfd = use_fd[0];
	int accepted = AcceptConnections(lfd); // Check listener for incoming connection requests
	
	for (auto& connection : as) {
		int result = ServiceConnection(connection.tcp, connection.fd);
		if (result == 0) {
			//Close
			continue;
		}

		//Reset connection
		connection.fd.revents = 0;
	}

	std::vector<int> to_drop;

	for (int i = use_fd.size() - 1; i >= 1; i--) {//Backwards to avoid decrementing durin closing connections

		//int result = ServiceConnection(connections[i - 1], use_fd[i]);
		int result = 0;
		int connection_idx = i - 1;
		TCPConnection& connection = connections[connection_idx];
		PacketManager& pm_write = connection.pm_write;
		PacketManager& pm_read = connection.pm_read;

		if (result == 0) {
			CloseConnection(connection_idx, std::string("Service Failed"));
		}

		if (use_fd[i].revents & POLLERR) {//Nothing to read
			CloseConnection(connection_idx, "POLLERR");
			continue;
		}
		if (use_fd[i].revents & POLLHUP) {//Nothing to read
			CloseConnection(connection_idx, "POLLHUP");
			continue;
		}
		if (use_fd[i].revents & POLLNVAL) {//Nothing to read
			CloseConnection(connection_idx, "POLLNVAL");
			continue;
		}

		if (use_fd[i].revents & POLLRDNORM) {//Can we non-blocking read ?
			//read data
			auto pm = pm_read;

			int bytes_recieved = 0;

			if (pm.current_task == PacketManagerTask::ProcessPacketSize) {
				//If this by chance only sends one of two bytes from uint16-> extraction offset will take care of it. 
				bytes_recieved = recv(use_fd[i].fd,
					(char*)&pm.current_packet_size + pm.current_packet_extraction_offset,
					sizeof(uint16_t) - pm.current_packet_extraction_offset,
					0);
			}
			else if (pm.current_task == PacketManagerTask::ProcessPacketContents) {
				bytes_recieved = recv(use_fd[i].fd,
					(char*)&connection.buffer + pm.current_packet_extraction_offset,
					pm.current_packet_size - pm.current_packet_extraction_offset,
					0);
			}


			if (bytes_recieved == 0) {
				CloseConnection(connection_idx, "0 bytes received");
				continue;
			}
			if (bytes_recieved == SOCKET_ERROR) {
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK) {//Close connection unless blocking-error
					CloseConnection(connection_idx, "Wouldblock error");
					continue;
				}
			}
			if (bytes_recieved < -1) {
				CloseConnection(connection_idx, "-" + std::to_string(bytes_recieved) + " bytes received");
				continue;
			}

			pm.current_packet_extraction_offset += bytes_recieved;

			if (pm.current_task == PacketManagerTask::ProcessPacketSize) {
				if (pm.current_packet_extraction_offset == sizeof(uint16_t)) {
					pm.current_packet_size = ntohs(pm.current_packet_size);
					if (pm.current_packet_size > max_packet_size) {
						CloseConnection(connection_idx, "Packet size too large");
						continue;
					}

					pm.current_packet_extraction_offset = 0;
					pm.current_task = PacketManagerTask::ProcessPacketContents;
				}
			}
			else if (pm.current_task == PacketManagerTask::ProcessPacketContents) {

				std::shared_ptr<Packet> packet = std::make_shared<Packet>();
				packet->buffer.resize(pm.current_packet_size);
				memcpy(&packet->buffer[0], connection.buffer, pm.current_packet_size);

				pm.Append(packet);

				pm.current_packet_size = 0;
				pm.current_packet_extraction_offset = 0;
				pm.current_task = PacketManagerTask::ProcessPacketSize;
			}
		}

		if (use_fd[i].revents & POLLWRNORM) {//Can we non-blocking write ? 
			//write data
			auto pm = pm_write;
			while (pm_write.HasPendingPackets()) {
				if (pm_write.current_task == PacketManagerTask::ProcessPacketSize) {
					pm_write.current_packet_size = pm.Retrieve()->buffer.size();
					uint16_t big_endian_packet_size = htons(pm_write.current_packet_size);
					int bytes_sent = send(use_fd[i].fd, 
											(char*)&big_endian_packet_size + pm.current_packet_extraction_offset, 
											sizeof(uint16_t) - pm.current_packet_extraction_offset, 
											0);
					if (bytes_sent > 0) {
						pm.current_packet_extraction_offset += bytes_sent;
					}

					if (pm.current_packet_extraction_offset == sizeof(uint16_t)){
						pm.current_packet_extraction_offset = 0;
						pm.current_task = PacketManagerTask::ProcessPacketContents;
					}
					else {
						break; //avoid sending half because of blocking
					}

				}
				else if (pm_write.current_task == PacketManagerTask::ProcessPacketContents) {
					char* buffer_ptr = &pm.Retrieve()->buffer[0];
					int bytes_sent = send(use_fd[0].fd,
											(char*)buffer_ptr + pm.current_packet_extraction_offset,
											pm.current_packet_size - pm.current_packet_extraction_offset, 
											0);
					if (bytes_sent > 0) {
						pm.current_packet_extraction_offset += bytes_sent;
					}

					if (pm.current_packet_extraction_offset == pm.current_packet_size) { //Sent entire packet contents
						pm.current_packet_extraction_offset = 0;
						pm.current_task = PacketManagerTask::ProcessPacketSize;
						pm.Pop();
					}
					else {
						break;
					}
				}
			}
		}


	}

	//Handle packets
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

	//Remove closed connections!

	return 1;
}


int Server::ServiceConnection(TCPConnection tcp, WSAPOLLFD fd) {


	//int errors = CheckFDErrors();
	if (fd.revents & POLLERR) {//Nothing to read
		return 0;
	}
	if (fd.revents & POLLHUP) {//Nothing to read
		return 0;
	}
	if (fd.revents & POLLNVAL) {//Nothing to read
		return 0;
	}
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
	master_fd.push_back(accepted_fd);

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