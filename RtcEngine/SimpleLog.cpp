#include "common.h"

static std::ofstream gLogFile;

int InitLogger(const char* logFileNamePrefix)
{
	if (!logFileNamePrefix)
	{
		return CodeFalse;
	}
	if (gLogFile.is_open())
	{
		return CodeFalse;
	}

	time_t now;
	time(&now);
	struct tm tmnow;
#ifdef _WIN32
	localtime_s(&tmnow, &now);
#else
	localtime_r(&now, &tmnow);
#endif

	std::stringstream ss;
	ss << logFileNamePrefix << '_' 
		<< std::setw(2) << std::setfill('0') << (tmnow.tm_mon + 1) << '_'
		<< std::setw(2) << std::setfill('0') << tmnow.tm_mday << ".log";

	gLogFile.open(ss.str(), std::ofstream::out | std::ofstream::app);
	if (!gLogFile.is_open())
	{
		return CodeFalse;
	}

	std::clog.rdbuf(gLogFile.rdbuf());

	return CodeOK;
}

static thread_local std::stringstream gSS;

LogPrintHelper::LogPrintHelper()
{
}

LogPrintHelper::~LogPrintHelper()
{
	auto ss = gSS.str();
	gSS.str("");

	time_t now;
	time(&now);
	struct tm tmnow;
#ifdef _WIN32
	localtime_s(&tmnow, &now);
#else
	localtime_r(&now, &tmnow);
#endif

#ifndef _DEBUG
	std::clog << std::setw(2) << std::setfill('0') << tmnow.tm_hour << ':'
		<< std::setw(2) << std::setfill('0') << tmnow.tm_min << ":"
		<< std::setw(2) << std::setfill('0') << tmnow.tm_sec << ' '
		<< ss << std::endl;
#else
	#ifdef _WIN32
		OutputDebugStringA(ss.c_str());
		OutputDebugStringA("\n");
	#endif
#endif
}

std::stringstream& LogPrintHelper::Log()
{
	return gSS;
}