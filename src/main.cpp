#include "Config.h"

#include <iostream>
#include <queue>
#include <mutex>

#include "Core.h"
#include "Graphics.h"

#include "Networking.h"
#include "Server.h"
#include "Client.h"
int parse_callback(char buffer[10]) {
    std::cout << "Callback function called!\n";
    std::cout << "Data : ";
    for (int i = 0; i < sizeof(buffer); i++) {
        std::cout << buffer[i];
    }
    std::cout << "\n";
    return 2;
}

int main(int argc, char* argv[]){
    std::cout << "Template VERSION " << Template_VERSION_MAJOR << "." << Template_VERSION_MINOR << "\n";
    int result = Networking::Intialize();
    if (result != 1) {
        std::cout << "Network failed intilizing Winsock\n";
    }

#pragma region Templated Parsing Server
    if (false) {
        std::vector<int> storage;
        ParseServer<std::vector<int>, int, char, 10> pserv;
        pserv.RegisterCommonStorage(&storage);
        pserv.RegisterCallbackFunction(*parse_callback);

        char bf[10];
        for (int i = 0; i < 10; i++) {
            bf[i] = 'a' + i;
        }
        pserv.CallCallback(bf);
    }
#pragma endregion

    Server server;
    std::thread server_thread(&Server::RunThreadedServer, &server, IPEndpoint("localhost", 8000));


    Client client;
    std::thread client_thread(&Client::Run, &client, IPEndpoint("localhost", 8000));

    server_thread.join();
    client_thread.join();

    Networking::Shutdown();
    std::cout << "Closed application...\n";
    return 0;
}
