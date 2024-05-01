#pragma once
#include <memory>
#include <vector>
#include "LoggerWindow.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Graphics {
private:
    GLFWwindow* window = nullptr;
    const char* glsl_version = "#version 130";

    std::vector<int> gui_elements;

    std::shared_ptr<LoggerWindow> logger_window;

public:

    Graphics();
    ~Graphics();

    int Init();

    int Run();
    int Render();

    int Shutdown();

    void SetLoggerWindow(std::shared_ptr<LoggerWindow> logger);
};
