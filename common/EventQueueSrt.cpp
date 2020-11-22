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

int EventQueueSrt::AddToEpoll(SRTSOCKET u, SrtFn&& fn, int ev)
{
	if (!fn)
	{
		return CodeFalse;
	}
	ev |= SRT_EPOLL_ERR;

	auto iter = m_mapSockets.find(u);
	if (iter == m_mapSockets.end())
	{
		auto res = srt_epoll_add_usock(m_srt_epoll, u, &ev);
		if (SRT_ERRNO::SRT_SUCCESS == res)
		{
			m_mapSockets.insert(std::make_pair(u, std::move(fn)));
		}
		else
		{
			LOG() << "srt_epoll_add_usock " << res;
		}
	}
	else
	{
		LOG() << "AddToEpoll exist " << u;
		auto res = srt_epoll_update_usock(m_srt_epoll, u, &ev);
		LOG() << "srt_epoll_update_usock " << res;
	}

	return CodeOK;
}

int EventQueueSrt::Pull()
{
	int res;
	const int kWaitTimeMS = 50;
	
	res = srt_epoll_uwait(m_srt_epoll, m_fdSockets, sizeof(m_fdSockets) / sizeof(m_fdSockets[0]), kWaitTimeMS);
	if (res < 0)
	{
		LOG() << "srt_epoll_uwait " << res;
		return CodeFalse;
	}
	else if (res == 0)
	{
		return CodeSrtEpoll_0;
	}

	auto iter = m_mapSockets.end();
	int ev;
	SRTSOCKET u = SRTT_INVALID;
	for (int i = 0; i < res; ++i)
	{
		ev = m_fdSockets[i].events;
		u = m_fdSockets[i].fd;
		iter = m_mapSockets.find(u);
		if (iter != m_mapSockets.end())
		{
			iter->second(ev);

			if (ev & SRT_EPOLL_ERR)
			{
				srt_epoll_remove_usock(m_srt_epoll, u);
				m_mapSockets.erase(iter);
			}
		}
	}

	return CodeOK;
}