# CPP_Template
This is a C++ template project. It implements some generic functionality for fast startup of new projects of different kinds. 

The vendor folder contains third-party packages : GLFW, IMGUI, GLAD, GLEW, EIGEM, GLM, SPDLOG, NLOHMANN/JSON, 
TODO LINKS TO THIRD PARTIES
- [Building](#building)
- [Running](#running)
- [Configuration](#configuration)
- [Modules](#modules)
	- [Networking](#configuration)
	- [Graphics](#graphics)
	- [Core](#core)
	- [Logger](#logger)

## Usage

main.cpp
```c++
#include "Config.h"

#include "Core.h"
#include "Graphics.h"
#include "Logger.h"

#include "Networking.h"
#include "Server.h"
#include "Client.h"

int main(int argc, char* argv[]){
    Logger::Initalize(L_ALL, L_GUI);
    Networking::Intialize();

    spdlog::info( "TEMPLATE VERSION " + std::to_string(Template_VERSION_MAJOR) + "." + std::to_string(Template_VERSION_MINOR));

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
Simple static library implementing a IPv4 non-blocking send and recieve server.

- Server
```cpp
#include "Server.h"

Server server;
server.Initialize(IPEndpoint("localhost", 8000));
server.SetPacketParsingCallback() - NOT IMPLEMENTED
while(true){
    server.Frame();
}
```
- Client
The client is blocking, thus run it in a thread. Setting the bool to false closes the client.
```cpp
Client client;
bool client_run = true;
std::thread client_thread(&Client::Run, &client, IPEndpoint("localhost", 8000), &client_run);
```   

## Graphics
Graphics is a static library with OpenGL, GLM, and ImGUI functionality. 
```cpp
Graphics graphics;
graphics.Init;
while(true){
    server.Render();
}
```

## Core
Core is a static library
To do: Genric JSON parsing

## Logger
A simple logger, use spdlog for GUI callback.
Usage:
```cpp
#include "Logger.h"

Logger::Initalize(L_ALL, L_CONSOLE); //Call before usage

Logger::Log( L_ERROR, "Reason" );
Logger::Log( L_INFO, "Reason" );
Logger::Log( L_DEBUG, "Reason" );
```
