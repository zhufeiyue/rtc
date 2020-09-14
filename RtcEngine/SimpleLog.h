#pragma once

#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "ErrorCode.h"

int InitLogger(const char*);

class LogPrintHelper
{
public:
	LogPrintHelper();
	~LogPrintHelper();
	std::stringstream& Log();
};

#ifdef LOG
#error redefine LOG
#else
#define LOG LogPrintHelper().Log
#endif