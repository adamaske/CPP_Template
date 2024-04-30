#pragma once
#include <mutex>
#include <vector>
#include "TCPConnection.h"
#include "Endpoint.h"
#include <map>
#include <utility>

struct Connection {
	TCPConnection tcp;
	WSAPOLLFD fd;
};

class Server {
private:

	//std::vector<int> callbacks;
public:
	Server();
	~Server();
	//Listen to endpoint and accept connections
	int Initialize(IPEndpoint endpoint);

	//One server tick
	int Frame();
	std::pair<int, std::string> ServiceConnection(TCPConnection& tcp, WSAPOLLFD& fd);
	int Read(WSAPOLLFD fd, PacketManager* pm, TCPConnection* tcp);
	int Write(WSAPOLLFD fd, PacketManager* pm, TCPConnection* tcp);
	int AcceptConnections(WSAPOLLFD listening_fd);
	int CloseConnection(int idx, std::string reason);

	int ProcessPacket(std::shared_ptr<Packet> packet);


	int OnConnect(TCPConnection& connection);
	int OnDisconnect(TCPConnection& connection, std::string reason);

	int Run();

public:
	//LISTENING --> 
	IPSocket listen_socket;
	WSAPOLLFD listen_fd;

	std::vector<TCPConnection> connections;
	std::vector<WSAPOLLFD> master_fd; 
	std::vector<WSAPOLLFD> use_fd;

	std::vector<Connection> as;
	//We have an array of connections, each connection is associated with a file descriptor

	std::map<int, TCPConnection> map_connections;
	std::map<int, WSAPOLLFD> map_fd;
};

template<typename StorageType, typename CallbackType, typename BufferType,int BufferSize>
class ParseServer {
private:
	StorageType* parsed_value_storage;
	//Function pointer -> Output 
	CallbackType (*callback_function)(BufferType buffer[BufferSize]);
	std::mutex* mutex;
public:
	void RegisterCommonStorage(StorageType* container) {
		parsed_value_storage = container;
	};

	void RegisterCallbackFunction(CallbackType (*callback)(BufferType buffer[BufferSize])) {
		callback_function = callback;
	};

	void CallCallback(BufferType buffer[BufferSize]) {

		(*callback_function)(buffer);
	};

	void RegisterMutex(std::mutex* m) {
		mutex = m;
	}
};