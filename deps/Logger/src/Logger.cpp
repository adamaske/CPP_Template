#include "Logger.h"

#include <iostream>
#include <chrono>
#include <ctime> 
#include <stdio.h>
#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static std::shared_ptr<Logger> logger_instance;
Logger::~Logger() {
    if (current_output == L_FILE) {

        output_file.close();
    }
}

void Logger::Initalize(LogLevel level, LogOutput output)
{
	//Populate the logger
    if (!logger_instance) {
        //Create new logger
        logger_instance = std::make_shared<Logger>();
    }

    logger_instance->current_level = level;
    logger_instance->current_output = output;

    switch (logger_instance->current_output) {
    case L_CONSOLE:
        //Do nothing
        break;
    case L_FILE:
        //Create file
        logger_instance->output_file = std::ofstream("log.txt");
        break;
    case L_GUI:
        //Graphics -> Create a logg window
        break;
    }
}
void Logger::Log(LogLevel level, std::string message)
{
    if (!logger_instance) {
        std::cout << "Logger not intialized!\n";
        return;
    }
    if (level != logger_instance->current_level && logger_instance->current_level != L_ALL) {
        return;
    }
    std::string level_string = logger_instance->level_text[level];
    std::string output = logger_instance->GetDateTime() + " " + level_string + " : " + message;

    switch (logger_instance->current_output) {
    case L_CONSOLE:
        std::cout << output << "\n";
        break;
    case L_FILE:
        if (!logger_instance->output_file.is_open()) {
            break;
        }
        logger_instance->output_file << output << std::endl;
        break;
    case L_GUI:
        //TODO : 
        break;
    }
}

std::string Logger::GetDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}