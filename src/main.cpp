#include "Config.h"
#include "Core.h"
#include "Logger.h"
#include "Graphics.h"
#include "Networking.h"
#include "Server.h"
#include "Client.h"

int main(int argc, char* argv[]){  
    Logger::Initalize(Logger::L_INFO, Logger::L_CONSOLE);
    Networking::Intialize();

    Logger::Info("Fundamentum VERSION " + std::to_string(Fundamentum_VERSION_MAJOR) + "." + std::to_string(Fundamentum_VERSION_MINOR));
    
    Graphics graphics;
    graphics.Init("Fundamentum");

    Server server;
    server.Initialize(IPEndpoint("localhost", 8000));

    int result = 1;
    while (result) {

        server.Frame(); //Runs the server
        result = graphics.Render(); //return 0 when window closes

    }

    graphics.Shutdown();
    Networking::Shutdown();
    return 0;
}
