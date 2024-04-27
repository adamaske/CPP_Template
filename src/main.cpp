#include <iostream>
#include "Core.h"

#include "Networking.h"

#include "Config.h"
#include "Graphics.h"
#include "Shader.h"

int main(int argc, char* argv[]){
   
    std::cout << "Template VERSION " << Template_VERSION_MAJOR << "." << Template_VERSION_MINOR << "\n";

    int result = 0;
    result = Networking::Intialize();
    if (result != 1) {
        std::cout << "Network failed intilizing Winsock\n";
    }

    // Core
    Core c;
    c.Init();

    // Graphics
    // OpenGLWindow w;
    // w.Init();

   
    // Networking


    Networking::TestNetwork();
    
    Networking::Shutdown();
    return 0;
}
