#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <map>

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

class Logger {
public:
	~Logger();
	static void Initalize(LogLevel level, LogOutput output);

	static void Log(LogLevel level, std::string message);

	std::string GetDateTime();

private:
	std::ofstream output_file;
	LogLevel current_level;
	LogOutput current_output;

	std::map<LogLevel, std::string> level_text = {	{L_NONE,	"[ NONE  ]"},
													{L_ERROR,	"[ ERROR ]"},
													{L_WARNING, "[ WARN  ]"},
													{L_INFO,	"[ INFO  ]"},
													{L_DEBUG,	"[ DEBUG ]"},
													{L_ALL,		"[ ALL   ]"} };
};