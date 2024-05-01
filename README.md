# CPP_Template
This is a C++ template project. It implements some generic functionality for fast startup of new projects of different kinds. 



The vendor folder contains third-party packages. 
- [Building](#building)
- [Running](#running)
- [Configuration](#configuration)
- [Modules](#modules)
	- [Networking](#configuration)
	- [Graphics](#graphics)
	- [Core](#core)
	- [Logger](#logger)
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
Networking is a static library. 
Only IPv4 is supported.
Implemented : Basic TCP server
To do: 
For simple and generic functionality : Encapsulate the blocking server in a thread and add received data to a mutexed queue 
- Server

- Client

## Graphics

Graphics is a static library with OpenGL, GLM, and ImGUI functionality. 

## Core
Core is a static library
To do: Genric JSON parsing

## Logger
A simple logger
Usage:
```cpp
#include "Logger.h"

Logger::Initalize(L_ALL, L_CONSOLE); //Call before usage

Logger::Log( L_ERROR, "Reason" );
Logger::Log( L_INFO, "Reason" );
Logger::Log( L_DEBUG, "Reason" );
```
