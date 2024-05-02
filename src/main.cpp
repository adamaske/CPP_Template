#include "Config.h"

#include "Core.h"
#include "Graphics.h"
#include "Logger.h"

#include "Networking.h"
#include "Server.h"
#include "Client.h"

#include <nlohmann/json.hpp>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/callback_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void InitLogger() {
    std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("TEMPLATE Logger");
    spdlog::set_default_logger(logger);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); //Console printing
    logger->sinks().push_back(console_sink);
}

int main(int argc, char* argv[]){
    InitLogger();

    Logger::Initalize(L_INFO, L_GUI);
    Networking::Intialize(),
    Logger::Log(L_INFO, "TEMPLATE VERSION " + std::to_string(Template_VERSION_MAJOR) + "." + std::to_string(Template_VERSION_MINOR));


    Graphics graphics;
    graphics.Init("Template"); //TODO : How to handle loggerwindow, its created here if Logger::OutputType = L_GUI

    Server server;
    server.Initialize(IPEndpoint("localhost", 8000));
    
    //Declare robot

    //Render

    int result = 1;
    while (result) {

        server.Frame(); //Runs the server
        result = graphics.Render(); //return 0 when window closes
    }

    graphics.Shutdown();
    Networking::Shutdown();
    return 0;
}
