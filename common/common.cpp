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

//static thread_local std::stringstream gSS;
LogPrintHelper::LogPrintHelper()
{
}

LogPrintHelper::~LogPrintHelper()
{
	m_ss << std::endl;
	auto logString = m_ss.str();
	//gSS.str("");

	std::clog << logString;

#ifndef _DEBUG
#ifdef _WIN32
	OutputDebugStringA(logString.c_str());
#endif
#endif
}

std::stringstream& LogPrintHelper::Log()
{
	time_t now;
	struct tm tmnow;

	time(&now);
#ifdef _WIN32
	localtime_s(&tmnow, &now);
#else
	localtime_r(&now, &tmnow);
#endif

	m_ss << std::setw(2) << std::setfill('0') << tmnow.tm_hour << ':'
		<< std::setw(2) << std::setfill('0') << tmnow.tm_min << ":"
		<< std::setw(2) << std::setfill('0') << tmnow.tm_sec << ' ';
	return m_ss;
}