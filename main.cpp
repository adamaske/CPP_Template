#include <iostream>

#include "Config.h"

int main(int argc, char* argv[]){

   std::cout << "Hello World!\n";

    // report version
    std::cout << "Version " << CPP_TEMPLATE_VERSION_MAJOR << "." << CPP_TEMPLATE_VERSION_MINOR << std::endl;
    if(argc < 2){

    }

    return 0;
}
