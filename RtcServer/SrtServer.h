#pragma once
#include <future>
#include <map>
#include "../common/srt/include/srt/srt.h"
#include "../common/common.h"

class SrtConnection
{
public:
	SrtConnection(SRTSOCKET);
	int Clear();

protected:
	SRTSOCKET m_clientSocket;
};

class SrtServer
{
public:
	int Create();
	int Destroy();

protected:
	int ServerLoop();
	SrtConnection* CreateConnectionHandler(SRTSOCKET u, int);

protected:
	SRTSOCKET m_socket = SRT_INVALID_SOCK;
	int m_srt_epoll = 0;
	int m_bServerLoopRunning = false;
	std::future<int> m_fuServer;
	std::map<SRTSOCKET, SrtConnection*> m_clients;
};
