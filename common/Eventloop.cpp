#include "Eventloop.h"

Eventloop::Eventloop()
{
	m_threadID = std::this_thread::get_id();
}

Eventloop::~Eventloop()
{
}

int Eventloop::Run()
{
	int res;
	Fn fn;

	m_threadID = std::this_thread::get_id();
	while (IsRunning())
	{
		res = m_srtEpoll.Pull();
		if (res == CodeFalse)
		{
			break;
		}

		res = m_eventQueue.PopEvent(fn);
		if (res == CodeOK)
		{
			fn();
			//continue;
		}
		res = m_eventQueueWorker.PopEvent(fn);
		if (res == CodeOK)
		{
			fn();
			continue;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return CodeOK;
}

int Eventloop::Quit()
{
	m_bRunning = false;
	return CodeOK;
}

int Eventloop::AddToEpoll(SRTSOCKET u, SrtFn fn, int flags)
{
	return QueueInLoop([=]() mutable
		{
			m_srtEpoll.AddToEpoll(u, std::move(fn), flags);
		}, false);
}

int Eventloop::RemoveFromEpoll(SRTSOCKET u)
{
	return QueueInLoop([=]()
		{
			m_srtEpoll.RemoveFromEpoll(u);
		}, false);
}

int Eventloop::QueueInLoop(Fn&& fn, bool bInWorkQueue)
{
	if (bInWorkQueue)
	{
		return m_eventQueueWorker.PushEvent(std::forward<Fn>(fn));
	}

	if (m_threadID == std::this_thread::get_id())
	{
		fn();
	}
	else
	{
		return m_eventQueue.PushEvent(std::forward<Fn>(fn));
	}
	return CodeOK;
}

EventLoopThread::EventLoopThread(Eventloop& loop):
	m_loop(loop)
{
}

EventLoopThread::~EventLoopThread()
{
}

int EventLoopThread::IsRunning()
{
	return m_bRunning && m_loop.IsRunning();
}

int EventLoopThread::Start()
{
	if (m_fuThread.valid())
	{
		return CodeFalse;
	}

	m_fuThread = std::async([this]() 
		{
			Fn fn;

			while (m_loop.IsRunning())
			{
				if (CodeOK == m_loop.WorkerEventQueue().PopEvent(fn))
				{
					fn();
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
			}
			return 0;
		});

	return CodeOK;
}

int EventLoopThread::Stop()
{
	m_bRunning = true;
	if (m_fuThread.valid())
	{
		m_fuThread.wait();
	}
	return CodeOK;
}