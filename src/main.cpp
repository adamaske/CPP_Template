#include "Config.h"

#include "Core.h"

#include "Graphics.h"

#include "Networking.h"
#include "Server.h"
#include "Client.h"

#include "Logger.h"

int main(int argc, char* argv[]){  
    Logger::Initalize(Logger::L_INFO, Logger::L_CONSOLE);
    Networking::Intialize();

    Logger::Info("Hei");

    return 0;
    Logger::Info("TEMPLATE VERSION " + std::to_string(Template_VERSION_MAJOR) + "." + std::to_string(Template_VERSION_MINOR));
    //spdlog::info("TEMPLATE VERSION " + std::to_string(Template_VERSION_MAJOR) + "." + std::to_string(Template_VERSION_MINOR));
    

    Graphics graphics;
    graphics.Init("Template");

    TestServer server;
    server.Initialize(IPEndpoint("localhost", 8000));

    int result = 1;
    while (result) {

        server.Frame(); //Runs the server
        result = graphics.Render(); //return 0 when window closes

        Sleep(500);
    }

    graphics.Shutdown();
    Networking::Shutdown();
    return 0;
}
