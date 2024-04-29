#pragma once
#include <mutex>
#include <vector>
#include "TCPConnection.h"
#include "Endpoint.h"

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

	int CheckForPollErrors(SHORT revent);
	int AcceptConnections(WSAPOLLFD listening_fd);
	int CloseConnection(int idx, std::string reason);

	int ProcessPacket(Packet& packet);

	int Run();
public:
	IPSocket listen_socket;

	std::vector<TCPConnection> connections;
	std::vector<WSAPOLLFD> master_fd; 
	std::vector<WSAPOLLFD> use_fd;
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