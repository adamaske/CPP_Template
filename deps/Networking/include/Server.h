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

	//Non-blocking server tick. 
	int Frame();
	int ServiceConnection(TCPConnection& tcp, WSAPOLLFD& fd);
	// listen - What fd was used for listener
	// fd - filedescriptors polled
	// poll amount - how many interactions were requested
	int Poll(WSAPOLLFD& listening_fd, std::vector<WSAPOLLFD>& fd_vector, int& poll_amount);

	int Read(TCPConnection& tcp, WSAPOLLFD& fd);
	//Return - Amount of bytes receieved
	int Read(WSAPOLLFD fd, PacketManager* pm, TCPConnection* tcp);

	int Write(WSAPOLLFD fd, TCPConnection& tcp);
	//Return - Amount of bytes sent
	int Write(WSAPOLLFD fd, PacketManager* pm, TCPConnection* tcp);

	int AcceptConnections(WSAPOLLFD listening_fd);

	virtual int ProcessPacket(std::shared_ptr<Packet> packet);

	virtual int OnConnect(TCPConnection& connection);
	virtual int OnDisconnect(TCPConnection& connection, std::string reason);

	int Run();

public:
	//LISTENING --> 
	IPSocket listen_socket;
	WSAPOLLFD listen_fd;

	std::vector<Connection> connections;
};

template<typename StorageType, typename CallbackType, typename BufferType,int BufferSize, typename ParseType>
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

	// Parse function
	void ParsePacket(Packet packet);

	ParseType CallCallback(BufferType buffer[BufferSize]) {

		(*callback_function)(buffer);

		return ParseType();
	};

	void RegisterMutex(std::mutex* m) {
		mutex = m;
	}
};

