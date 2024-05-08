#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <ctime>
#include <chrono>

#ifdef USE_SPDLOG
#include "spdlog/spdlog.h"
#include "spdlog/sinks/callback_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#endif


namespace Logger { //Logger Wrapper - If spdlog is present use it

	enum LogLevel {
		L_INFO,
		L_DEBUG,
		L_WARN,
		L_ERROR,
		L_INVALID
	};

	enum LogOutput {
		L_CONSOLE,
		L_FILE,
		L_GUI
	};

	struct Log {
		LogLevel level = L_INFO; 
		
		std::chrono::system_clock::time_point time_point;
		std::string time_string = "";

		std::string payload = "";
	};

	static LogLevel current_level = L_INFO;
	static LogOutput current_output = L_CONSOLE;
	static std::vector<std::function<void(const Log)>> log_callback_functions;

#pragma region Helpers

	static std::string LevelToString(LogLevel level) {
		switch (level) {
		case L_INFO:
			return "INFO";
			break;
		case L_DEBUG:
			return "DEBUG";
			break;
		case L_WARN:
			return "WARN";
			break;
		case L_ERROR:
			return "ERROR";
			break;
		default:
			return "INVALID";
			break;
		}	
	}

	static std::string TimePointToString(std::chrono::system_clock::time_point time) {
		std::time_t t = std::chrono::system_clock::to_time_t(time);
		char buf[20];
		strftime(buf, 20, "%H:%M:%S", localtime(&t));
		return std::string(buf);
	}

#ifdef USE_SPDLOG
	static LogLevel SPDLOG_LevelToLogLevel(spdlog::level::level_enum level) {
		switch (level) {//LEVEL TO LEVEL
		case spdlog::level::info:
			return L_INFO;
			break;
		case spdlog::level::debug:
			return L_DEBUG;
			break;
		case spdlog::level::warn:
			return L_WARN;
			break;
		case spdlog::level::err:
			return L_ERROR;
			break;
		default:
			return L_INVALID;
			break;
		}
	}
#endif

	static std::string LogToString(const Logger::Log log) {
		return log.time_string + " [" + LevelToString(log.level) + "] " + log.payload;
	}

	static void ConsolePrintLog(const Log log) {
		std::cout << LogToString(log) << "\n";
	}

#pragma endregion

#pragma region CALLBACKS
#ifdef USE_SPDLOG //This is called each time spdlog logs
	static void SPDLOG_Callback(const spdlog::details::log_msg& msg) {
		Log log = {};
		
		log.level = SPDLOG_LevelToLogLevel(msg.level);
		log.time_point = msg.time;
		log.time_string = TimePointToString(log.time_point);

		log.payload = std::string(msg.payload.data());

		for (auto& callback : log_callback_functions) {
			callback(log);
		}
	}
#endif

	static void RegisterCallback(std::function<void(const Log)> callback) {
		log_callback_functions.push_back(callback);
	}
#pragma endregion

	static void Initalize(LogLevel level, LogOutput output) {
		current_level = level;
		current_output = output;

#ifdef USE_SPDLOG
		std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("Main");
		spdlog::set_default_logger(logger);

		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); //Console printing
		logger->sinks().push_back(console_sink);

		auto callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>(Logger::SPDLOG_Callback); //Setup callback sink
		spdlog::get("Main")->sinks().push_back(callback_sink);
#endif

		switch (current_output) {
		case L_CONSOLE:

			break;
		case L_FILE:
			//Start file
			break;
		case L_GUI:

			break;
		}
	}

#define ENFORCE_STD_STRING_LOGGING
#ifdef ENFORCE_STD_STRING_LOGGING

	inline void HandleLog(LogLevel level, const std::string& msg) {
		Log log = {};
		log.level = level;
		log.time_point = std::chrono::system_clock::now();
		log.time_string = TimePointToString(log.time_point);
		log.payload = msg;

		for (auto& callback : log_callback_functions) {
			callback(log);
		}

		switch (current_output) {
		case L_CONSOLE:
			ConsolePrintLog(log);
			break;
		case L_FILE:

			break;
		case L_GUI:

			break;
		}
	}

	inline void Info(const std::string& msg) {
#ifdef USE_SPDLOG
		spdlog::info(msg);
		return;
#endif
		HandleLog(L_INFO, msg);
	}

	inline void Debug(const std::string& msg) {
#ifdef USE_SPDLOG
		spdlog::debug(msg);
		return;
#endif
		HandleLog(L_DEBUG, msg);
	}

	inline void Warning(const std::string& msg) {
#ifdef USE_SPDLOG
		spdlog::warn(msg);
		return;
#endif
		HandleLog(L_WARN, msg);
	}

	inline void Error(const std::string& msg) {
#ifdef USE_SPDLOG 
		spdlog::error(msg);
		return;
#endif
		HandleLog(L_ERROR, msg);
	}

#else

	template<typename T>
	inline Log ConstructLog(LogLevel level, const T& msg) {
		Log log = {};
		log.level = level;
		log.time_point = std::chrono::system_clock::now();
		log.time_string = TimePointToString(log.time_point);
		log.payload = "UNIMPLEMENTED";
		return log;
	}

	template<>
	inline Log ConstructLog(LogLevel level, const std::string& msg) {
		Log log = {};
		log.level = level;
		log.time_point = std::chrono::system_clock::now();
		log.time_string = TimePointToString(log.time_point);
		log.payload = msg;
		return log;
	}

	template<typename T>
	inline void HandleLog(LogLevel level, const T& msg) {
		Log log = ConstructLog(level, msg);

		for (auto& callback : log_callback_functions) {
			callback(log);
		}

		switch (current_output) {
		case L_CONSOLE:
			ConsolePrintLog(log);
			break;
		case L_FILE:

			break;
		case L_GUI:

			break;
		}
	}

	template<typename T>
	inline void Info(const T& msg) {
#ifdef USE_SPDLOG
		spdlog::info(msg);
		return;
#endif
		HandleLog(L_INFO, msg);
	}

	template<typename T>
	inline void Debug(const T& msg) {
#ifdef USE_SPDLOG
		spdlog::debug(msg);
		return;
#endif
		HandleLog(L_DEBUG, msg);
	}

	template<typename T>
	inline void Warning(const T& msg) {
#ifdef USE_SPDLOG
		spdlog::warn(msg);
		return;
#endif
		HandleLog(L_WARN, msg);
	}

	template<typename T>
	inline void Error(const T& msg) {
#ifdef USE_SPDLOG && SPDLOG_ENABLED
		spdlog::error(msg);
		return;
#endif
		HandleLog(L_ERROR, msg);
	}

#endif
}