# Fundamentum
Fundamentum is a C++ library for fast startup of new projects.  

# TODO
- [ ] Change toplevel CMakeLists for submodule usage
- [ ] UE5 plugin for server interfacing
- [ ] Research plotting libraries
- [ ] Change vendor subfolder to git submodules (keep the manual ones by renaming vendor to old_vendor)

# Content
- [Building](#building)
- [Running](#running)
- [Configuration](#configuration)
- [Modules](#modules)
	- [Networking](#configuration)
	- [Graphics](#graphics)
	- [Core](#core)
    - [Logger](#logger)
- [Third-Party Packages](#third-party-packages)

# Usage
 
main.cpp
```c++
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

    Logger::Info("TEMPLATE VERSION " + std::to_string(Template_VERSION_MAJOR) + "." + std::to_string(Template_VERSION_MINOR));
    
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
```

# Building
Cmake must be installed for building the project. 
scripts/generate.bat can automatically build the project with cmake 
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

# Running
```bash
./src/VERSION/Template.exe
```

# Configuration
[CMakeLists.txt](CMakeLists.txt)
`project(YOUR_PROJECT_NAME_HERE)`

[src/Config.h.in](src/Config.h.in)
`#define YOUR_PROJECT_NAME_VERSION_MAJOR @YOUR_PROJECT_NAME_VERSION_MAJOR@`
`#define YOUR_PROJECT_NAME_VERSION_MINOR @YOUR_PROJECT_NAME_VERSION_MINOR@`

# Modules
/deps/...

## Networking
- [ ] IPv6 networking
- [ ] Non-blocking client

Static IPv4 Winsock networking library.  

### Server
```cpp
#include "Server.h"

Server server;
server.Initialize(IPEndpoint("localhost", 8000));
while(true){
    server.Frame();
}
```
### Client

Scripts/tcp_client.py implements a simple client with string-packet parsing and creation. 
```cpp
#include "Client.h"
 Client client;
 client.Connect(IPEndpoint("localhost", 12345));
 while (true) {
     client.Frame();
 }
```

## Graphics
Graphics is a static library utilizing GLFW, GLAD, GLM, and ImGUI.
```cpp
Graphics graphics;
graphics.Init("Template");
while(true){
    server.Render();
}
```

## Logger
Logger is a static library.  If CMake library 'SPDLOG' is found, then this is a wrapper for SPDLOG. If no SPDLOG library is found, this is a simple logging library. 
```cpp
#include "Logger.h"
Logger::Initalize(L_INFO, L_CONSOLE)
Logger::RegisterCallback([](const Logger::Log log){ std::cout << log.payload << std::endl;})
Logger::Info("Something happend!");
Logger::Error("An error occured...");
```

# Third-Party Packages

- [Spdlog Logger](https://github.com/gabime/spdlog)

- [NLOHMANN JSON](https://github.com/nlohmann/json)

- [GLFW](https://www.glfw.org/)

- [GLM](https://github.com/g-truc/glm)

- [ImGui](https://github.com/ocornut/imgui)

- [Eigen](https://gitlab.com/libeigen/eigen)

- [GLAD](https://github.com/Dav1dde/glad)

- [GLEW](https://github.com/nigels-com/glew)
