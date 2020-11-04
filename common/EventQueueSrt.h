#pragma once

#include "EventQueue.h"
#include "common.h"

class EventQueueSrt 
{
public:
	EventQueueSrt();
	~EventQueueSrt();
	int GetEpollID() { return  m_srt_epoll; }
	int AddToEpoll(SRTSOCKET, int );


private:
	int m_srt_epoll = -1;
};

