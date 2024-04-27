#include "Server.h"
#include <iostream>

Server::Server()
{
	std::cout << "Server Constructed\n";
}

Server::~Server()
{
	std::cout << "Server Deconstructed\n";
}

int Server::RegisterCallback()
{
	std::cout << "Server Register\n";
	return 0;
}
