#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>
#include <string>

#include "LoggerWindow.h"
#include "GUIWindow.h"

class Graphics {
private:
    GLFWwindow* window = nullptr;
    const char* glsl_version = "#version 130";

    std::vector<int> gui_elements;

    std::shared_ptr<LoggerWindow> logger_window;

    std::vector<std::shared_ptr<GUIWindow>> gui_windows;
public:

    Graphics();
    ~Graphics();

    int Init(std::string title);

    int Frame();

    int Render();

    int Shutdown();

    void AppendGUIWindow(std::shared_ptr<GUIWindow> gui_window);

};
