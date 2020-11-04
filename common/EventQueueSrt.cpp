#include "srt/include/srt/srt.h"
#include "EventQueueSrt.h"

EventQueueSrt::EventQueueSrt()
{
	m_srt_epoll = srt_epoll_create();
	LOG() << "srt_epoll_create " << m_srt_epoll;
	int res = srt_epoll_set(m_srt_epoll, SRT_EPOLL_ENABLE_EMPTY);
	LOG() << "srt_epoll_set " << m_srt_epoll << " SRT_EPOLL_ENABLE_EMPTY " << res;
	if (res == -1)
	{
		srt_epoll_release(m_srt_epoll);
		m_srt_epoll = -1;
	}
}

EventQueueSrt::~EventQueueSrt()
{
	if (m_srt_epoll != -1) 
	{
		srt_epoll_clear_usocks(m_srt_epoll);
		srt_epoll_release(m_srt_epoll);
		m_srt_epoll = -1;
	}
}