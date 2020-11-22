#pragma once

#include "srt/include/srt/srt.h"
#include "EventQueue.h"

typedef std::function<void(int)> SrtFn;

class EventQueueSrt 
{
public:
	EventQueueSrt();
	~EventQueueSrt();
	int GetEpollID() { return  m_srt_epoll; }
	int AddToEpoll(SRTSOCKET, SrtFn&& , int );
	int Pull();

private:
	int m_srt_epoll = -1;
	SRT_EPOLL_EVENT m_fdSockets[100];
	std::map<SRTSOCKET, SrtFn> m_mapSockets;
};

