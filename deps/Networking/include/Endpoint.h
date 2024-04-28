#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <WS2tcpip.h>

struct IPEndpoint {
	std::string hostname = "";
	std::string ip_string = "";
	std::vector<uint8_t> ip_bytes;
	unsigned short port;

	IPEndpoint(const char* ip, unsigned short _port) {
		port = _port;

		in_addr addr; //inet_ "presentation format a.k.a "127.0..." to "network format"
		int result = inet_pton(AF_INET, ip, &addr);
		if (result == 1) {
			if (addr.S_un.S_addr != INADDR_NONE) {
				hostname = ip;
				ip_string = ip;
				//Ip version
				ip_bytes.resize(sizeof(ULONG));
				//We know that an IPv4 ip address is 4 bytes long, a ULONG is 4 bytes, thus we can copy
				memcpy(&ip_bytes[0], &addr.S_un.S_addr, sizeof(ULONG));
				return;
			}
		}
		
		//Resolve hostname
		addrinfo hints = {};//Filter for ipv4 only
		hints.ai_family = AF_INET; // only ipv4 only
		addrinfo* hostinfo = nullptr; //Pointer to linked list
		result = getaddrinfo(ip, NULL, &hints, &hostinfo);
		if (result != 0) {
			std::cout << "Could not resolve hostname to IPv4 : " << WSAGetLastError() << "\n";
			return;
		}
		sockaddr_in* host_addr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);
		hostname = ip;
		ip_string.resize(16);
		inet_ntop(AF_INET, &host_addr->sin_addr, &ip_string[0], 16);

		ULONG ip_long = host_addr->sin_addr.S_un.S_addr;
		ip_bytes.resize(sizeof(ULONG));
		memcpy(&ip_bytes[0], &ip_long, sizeof(ULONG));

		freeaddrinfo(hostinfo); // free linked list
	}

	sockaddr_in GetSockaddr() {
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr.S_un.S_addr, &ip_bytes[0], sizeof(ULONG)); //Populate the addres with our ip_bytes
		addr.sin_port = htons(port);//swap endianess, host to network-byte-order short
		return addr;
	}

	void PrintEndpoint() {
		std::cout << "Hostname : " << hostname << "\n";
		std::cout << "IP : " << ip_string << "\n";
		std::cout << "Port : " << port << "\n";
		std::cout << "IP Bytes : ";
		for (auto& digit : ip_bytes) {
			std::cout << int(digit) << ".";
		}
		std::cout << "\n";
	};
	std::string ShortDesc() {
		return ip_string + " : " + std::to_string(port);
	}
}; 