#pragma once

#include <ctime>
#include <cassert>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <functional>
#include <sstream>
#include <thread>
#ifdef _WIN32
#include <Windows.h>
#endif

//int InitLogger(const char*);

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

enum ErrorCode
{
	CodeInternalError = -1,
	CodeOK = 0,
	CodeFalse
};

struct WaitSomeTime
{
	WaitSomeTime(long long t)
	{
		if (t < 0)
			t = 1000000000 / 100;
		timeWaitGap = std::chrono::nanoseconds(t);
		timeWaitModify = std::chrono::nanoseconds(0);
		timeNow = std::chrono::steady_clock::now();
		timeNext = timeNow + timeWaitGap;
	}

	void Wait()
	{
		std::this_thread::sleep_until(timeNext - timeWaitModify);
		timeNow = std::chrono::steady_clock::now();
		timeWaitModify = timeNow - timeNext;
		timeNext += timeWaitGap;
	}

	std::chrono::nanoseconds timeWaitGap;
	std::chrono::nanoseconds timeWaitModify;
	std::chrono::steady_clock::time_point timeNow;
	std::chrono::steady_clock::time_point timeNext;
};

#ifdef _WIN32

int InitMFEnv();
int DestroyMFEnv();

#endif