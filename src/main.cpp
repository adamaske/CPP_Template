#include "Config.h"

#include <iostream>
#include <queue>
#include <mutex>

#include "Core.h"
#include "Graphics.h"

#include "Networking.h"
#include "Server.h"
#include "Client.h"

#include "Logger.h"
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
    Logger::Initalize(L_ALL, L_CONSOLE);
    Networking::Intialize();

    Logger::Log(L_INFO, "Template VERSION " + std::to_string(Template_VERSION_MAJOR)  + "." + std::to_string(Template_VERSION_MINOR));
    
#pragma region Templated Parsing Server
    if (false) {
        std::vector<int> storage;
        ParseServer<std::vector<int>, int, char, 10, int> pserv;
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
    server.Initialize(IPEndpoint("localhost", 8000));
    std::thread server_thread(&Server::Run, &server);

    Client client;
    bool client_run = false;
    std::thread client_thread(&Client::Run, &client, IPEndpoint("localhost", 8000), &client_run);
    while (true) {
        std::string user = "";
        std::getline(std::cin, user);
        if (user == "s") {
            client_run = !client_run;
        }
        if (user == "q") {
            client_run = false;

            break;
        }
    }
    //client_thread.join();
    
    Networking::Shutdown();

    Logger::Log(L_INFO, "Closed application...");
    return 0;
}
