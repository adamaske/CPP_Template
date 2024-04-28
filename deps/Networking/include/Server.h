#pragma once
#include "Endpoint.h"

#include <mutex>
#include <vector>
class Server {
private:

	//std::vector<int> callbacks;
public:
	Server();
	~Server();

	int RunThreadedServer(IPEndpoint endpoint);
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