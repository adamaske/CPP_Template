#include "Client.h"
#include <iostream>

Client::Client() {
    std::cout << "Client Constructed\n";
}

Client::~Client() {
    std::cout << "Client Deconstructed\n";
}

void Client::Start(){
    std::cout << "Client start\n";
}