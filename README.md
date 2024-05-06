# CPP_Template
This is a C++ template project. It implements some generic functionality for fast startup of new projects of different kinds. 

TODO Change structre for easier git submodule

- [Building](#building)
- [Running](#running)
- [Configuration](#configuration)
- [Modules](#modules)
	- [Networking](#configuration)
	- [Graphics](#graphics)
	- [Core](#core)
-[Third-Party Packages](#Third-Party Packages)

## Usage
 
main.cpp
```c++
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
    Networking::Intialize();

    spdlog::info("TEMPLATE VERSION " + std::to_string(Template_VERSION_MAJOR) + "." + std::to_string(Template_VERSION_MINOR));

    Graphics graphics;
    graphics.Init("Template");

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

```

## Building
Cmake must be installed for building the project. 
scripts/generate.bat can automatically build the project with cmake 
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running
```bash
./src/VERSION/Template.exe
```

## Configuration
Make it your own by configuring it.

CMakeLists.txt : `project(YOUR_PROJECT_NAME_HERE)`
src/config.h.in :
`#define YOUR_PROJECT_NAME_VERSION_MAJOR @YOUR_PROJECT_NAME_VERSION_MAJOR@` and
`#define YOUR_PROJECT_NAME_VERSION_MINOR @YOUR_PROJECT_NAME_VERSION_MINOR@`

## Modules
Several static libaries are implemented in deps/.



## Networking
IPv4 TCP server static library. The server is non-blocking and uses packets. 
Scripts/tcp_client.py implements a simple client with packet parsing and creation. 

Server::use_packets changes bevhaiour to raw read and writing. 

- Server
```cpp
#include "Server.h"

Server server;
server.Initialize(IPEndpoint("localhost", 8000));
while(true){
    server.Frame();
}
```
- Client
TODO : Non blocking client
Scripts/tcp_client.py implements a simple client with string-packet parsing and creation. 

## Graphics
Graphics is a static library utilizing OpenGL-GLFW, GLM, and ImGUI.
```cpp
Graphics graphics;
graphics.Init("Template");
while(true){
    server.Render();
}
```

## Third-Party Packages

-[Spdlog C++ Logger](https://github.com/gabime/spdlog)

-[NLOHMANN C++ JSON](https://github.com/nlohmann/json)
-[GLFW](https://www.glfw.org/)
-[GLM](https://github.com/g-truc/glm)
-[ImGui](https://github.com/ocornut/imgui)
-[Eigen C++ Math](https://gitlab.com/libeigen/eigen)
-[GLAD](https://github.com/Dav1dde/glad)
-[GLEW](https://github.com/nigels-com/glew)
