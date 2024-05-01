#include "Config.h"

#include "Core.h"
#include "Graphics.h"
#include "Logger.h"

#include "Networking.h"
#include "Server.h"
#include "Client.h"

int main(int argc, char* argv[]){
    Logger::Initalize(L_ALL, L_GUI);
    Networking::Intialize(),
    Logger::Log(L_INFO, "TEMPLATE VERSION " + std::to_string(Template_VERSION_MAJOR) + "." + std::to_string(Template_VERSION_MINOR));


    Graphics graphics;
    graphics.SetLoggerWindow(Logger::GetWindow());
    graphics.Init();

    Server server;
    server.Initialize(IPEndpoint("localhost", 8000));
    
    Client client;
    bool client_run = true;
    std::thread client_thread(&Client::Run, &client, IPEndpoint("localhost", 8000), &client_run);
    
    int result = 1;
    while (result) {
        server.Frame(); //Runs the server
        result = graphics.Render(); //return 0 when window closes
    }

    client_thread.join();

    //client_run = false;
    Networking::Shutdown();
    return 0;
}
