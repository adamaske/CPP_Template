# CPP_Template
This is a C++ template project. It implements some generic functionality for fast startup of new projects of different kinds. 

Cmake must be installed for building the project. 
scripts/generate.bat can automatically build the project with cmake 

Networking is a static library. 
Implemented : Basic TCP server
To do: 
For simple and generic functionality : Encapsulate the blocking server in a thread and add received data to a mutexed queue 

Graphics is a static library with OpenGL, GLM, and ImGUI functionality. 

Core is a static library
To do: Genric JSON parsing

The vendor folder contains third-party packages. 
- [Building](#building)
- [Running](#running)
- [Configuration](#configuration)


## Building
mkdir build
cd build
cmake ..
cmake --build .

## Running
./src/VERSION/Template.exe

## Configuration