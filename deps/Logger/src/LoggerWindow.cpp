#include "LoggerWindow.h"

#include "Logger.h"



#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

ImGuiTextBuffer     Buf;
ImGuiTextFilter     Filter;
ImVector<int>       LineOffsets;        // Index to lines offset
bool                ScrollToBottom;

void LoggerWindow::AppendLog(LogElement log)
{
	std::string log_string = Logger::LogToString(log) + "\n";
	const char* log_cstr = log_string.c_str();

    va_list args = { };
    
    Buf.append(log_cstr, args);

    ScrollToBottom = true;
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

    ImGui::TextUnformatted(Buf.begin());

    if (ScrollToBottom) {
        ImGui::SetScrollHereX(1.0f);
        ImGui::SetScrollHereY(1.0f);
    }
    ScrollToBottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();

	return 0;
}

void LoggerWindow::Clear()
{
	Buf.clear(); 
	LineOffsets.clear();
}
