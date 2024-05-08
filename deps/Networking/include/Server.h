#pragma once
#include <mutex>
#include <vector>
#include "TCPConnection.h"
#include "Endpoint.h"
#include <map>
#include <utility>

class Server {
private:
	bool use_packets = true;
	IPSocket listen_socket;
	WSAPOLLFD listen_fd;

	std::vector<std::shared_ptr<TCPConnection>> tcps;
	std::vector<WSAPOLLFD> master_fds;
	std::vector<WSAPOLLFD> use_fds;
public:
	Server();
	~Server();

	int Initialize(IPEndpoint endpoint);

	int Frame();

private:
	int Poll(WSAPOLLFD& listening_fd, std::vector<WSAPOLLFD>& fd_vector, int& poll_amount);

	int AcceptConnections(WSAPOLLFD listening_fd);
	int ServiceConnection(std::shared_ptr<TCPConnection> tcp, WSAPOLLFD& fd);

	int Read(std::shared_ptr<TCPConnection> tcp, WSAPOLLFD& fd);
	int Write(std::shared_ptr<TCPConnection> tcp, WSAPOLLFD fd);

protected:
	virtual int ProcessPacket(std::shared_ptr<Packet> packet);
	virtual int OnConnect(std::shared_ptr<TCPConnection> connection);
	virtual int OnDisconnect(std::shared_ptr<TCPConnection> connection, std::string reason);

#pragma region Callbacks
	int RegisterCallback();

#pragma endregion
};

class TestServer : public Server {
private:


	//int ProcessPacket(std::shared_ptr<Packet> packet) override;
	//int OnConnect(std::shared_ptr<TCPConnection> connection) override;
	//int OnDisconnect(std::shared_ptr<TCPConnection> connection, std::string reason) override;
};