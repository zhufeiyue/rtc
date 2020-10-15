#pragma once
#include <future>
#include "srt/include/srt/srt.h"
#include "common.h"

class SrtClient
{
public:
	int Connect(std::string address, int16_t port);
	int DisConnect();

protected:
	int ClientLoop();

protected:
	void OnConnect(int);
	void OnDisConnect(int);

protected:
	SRTSOCKET m_socket = SRT_INVALID_SOCK;
	int m_srt_epoll = 0;
	int m_bClientLoopRunning = false;
	std::future<int> m_fuClient;
};
