#pragma once
#include <string>
enum LogLevel {
	L_NONE,
	L_ERROR,
	L_WARNING,
	L_INFO,
	L_DEBUG,
	L_ALL
};

enum LogOutput {
	L_CONSOLE = 0,
	L_FILE = 1,
	L_GUI = 2
};



struct LogElement {
	std::string date_time;
	LogLevel level;
	std::string message;
};
