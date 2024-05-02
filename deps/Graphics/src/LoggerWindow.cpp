#include "LoggerWindow.h"

#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/callback_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"


void LoggerWindow::LoggerWindowCallback() {
   
}

LoggerWindow::LoggerWindow() {
    auto callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>
        ([this](const spdlog::details::log_msg& msg) {
        
        auto time_point = msg.time;
        std::time_t time = std::chrono::system_clock::to_time_t(time_point);
        std::string time_string = std::ctime(&time);
        time_string.resize(time_string.size() - 1);

        std::string level_string(spdlog::level::to_short_c_str(msg.level));
        std::string payload_string(msg.payload.data());
        std::string text = "[" + level_string + "] " + payload_string + "\n";

        va_list args = { };
        buf.appendfv(text.c_str(), args);

        scroll_to_bottom = true;
    });
    //callback_sink->set_level(spdlog::level::info);
    spdlog::get("Main")->sinks().push_back(callback_sink);
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
