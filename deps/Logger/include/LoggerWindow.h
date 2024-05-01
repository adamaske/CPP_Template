#pragma once


#include "LoggerConstants.h"
#include <string>
#include <vector>

class LoggerWindow {
public:
	void AppendLog(LogElement log);

	int Render();

	void Clear();

};