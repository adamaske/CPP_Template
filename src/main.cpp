#include "Config.h"

#include "Core.h"

#include "Graphics.h"

#include "Networking.h"
#include "Server.h"
#include "Client.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void InitLogger() {
    std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("Main");
    spdlog::set_default_logger(logger);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); //Console printing
    logger->sinks().push_back(console_sink);
}

int main(int argc, char* argv[]){
    InitLogger();

    Networking::Intialize(),
    spdlog::info("TEMPLATE VERSION " + std::to_string(Template_VERSION_MAJOR) + "." + std::to_string(Template_VERSION_MINOR));

    Graphics graphics;
    graphics.Init("Template"); //TODO : How to handle loggerwindow, its created here if Logger::OutputType = L_GUI

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
