#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <map>

#include "LoggerConstants.h"

class Logger {
public:
	~Logger();
	static void Initalize(LogLevel level, LogOutput output);

	static void Log(LogLevel level, std::string message);

	static std::string GetDateTime();

	static std::string LogToString(LogElement log);

	static void RegisterCallbackFunction(void(*callback)(LogElement log));

	static LogOutput GetOutputType();

private:
	std::ofstream output_file;
	LogLevel current_level = L_ALL;
	LogOutput current_output = L_CONSOLE;

	std::map<LogLevel, std::string> level_text = {	{L_NONE,	"[ NONE  ]"},
													{L_ERROR,	"[ ERROR ]"},
													{L_WARNING, "[ WARN  ]"},
													{L_INFO,	"[ INFO  ]"},
													{L_DEBUG,	"[ DEBUG ]"},
													{L_ALL,		"[ ALL   ]"} };

	
	std::vector<LogElement> logs;

	std::vector<void (*)(LogElement)> callbacks;
};