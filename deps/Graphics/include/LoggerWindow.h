#pragma once
#include "GUIWindow.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class LoggerWindow : public GUIWindow{
public:

	LoggerWindow();

	void LoggerWindowCallback();

	int Render() override;

	void Clear();

	ImGuiTextBuffer     buf;
	ImGuiTextFilter     filter;
	ImVector<int>       line_offsets;        // Index to lines offset
	bool                scroll_to_bottom = true;
};