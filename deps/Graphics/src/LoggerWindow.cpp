#include "LoggerWindow.h"

#include <iostream>

#include "Logger.h"
void LoggerWindow::LoggerWindowCallback() {
   
}

LoggerWindow::LoggerWindow() {

    Logger::RegisterCallback([this](const Logger::Log log) {

        std::string text = log.payload;
    
        va_list args = { };
        buf.appendfv(text.c_str(), args);
    
        scroll_to_bottom = true;
    });
}

int LoggerWindow::Render()
{
    ImGui::SetNextWindowSize(ImVec2(500, 400), 1);
    bool ser = true;
    ImGui::Begin("Logs", &ser);
    if (ImGui::Button("Clear")) {
        Clear();
    }

    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::Separator();
    ImGui::BeginChild("scrolling");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

    if (copy) {
        ImGui::LogToClipboard();
    }

    ImGui::Text(buf.begin());

    if (scroll_to_bottom) {
        ImGui::SetScrollHereX(1.0f);
        ImGui::SetScrollHereY(1.0f);
    }

    scroll_to_bottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();

	return 0;
}

void LoggerWindow::Clear()
{
	buf.clear(); 
	line_offsets.clear();
}
