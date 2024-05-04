#pragma once
#include <mutex>
#include <vector>
#include "TCPConnection.h"
#include "Endpoint.h"
#include <map>
#include <utility>

class Server {
private:

	//std::vector<int> callbacks;
public:
	Server();
	~Server();
	//Listen to endpoint and accept connections
	int Initialize(IPEndpoint endpoint);

	//Non-blocking server tick. 
	int Frame();
	// listen - What fd was used for listener
	// fd - filedescriptors polled
	// poll amount - how many interactions were requested
	int Poll(WSAPOLLFD& listening_fd, std::vector<WSAPOLLFD>& fd_vector, int& poll_amount);

	int AcceptConnections(WSAPOLLFD listening_fd);

	int ServiceConnection(TCPConnection& tcp, WSAPOLLFD& fd);

	int Read(TCPConnection& tcp, WSAPOLLFD& fd);

	int Write(TCPConnection& tcp, WSAPOLLFD fd);


	virtual int ProcessPacket(std::shared_ptr<Packet> packet);

	virtual int OnConnect(TCPConnection& connection);
	virtual int OnDisconnect(TCPConnection& connection, std::string reason);

public:
	bool use_packets = true;
	//LISTENING --> 
	IPSocket listen_socket;
	WSAPOLLFD listen_fd;

	std::vector<TCPConnection> tcps;
	std::vector<WSAPOLLFD> master_fds;
	std::vector<WSAPOLLFD> use_fds;
};
