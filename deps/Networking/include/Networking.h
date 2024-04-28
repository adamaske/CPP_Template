#pragma once

#include <queue>
#include <mutex>
#include "Endpoint.h"

class Networking{
    private:

    public:

    Networking();
    ~Networking();

    static bool Intialize();
    static void Shutdown();

};

