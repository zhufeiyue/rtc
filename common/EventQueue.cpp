#include "EventQueue.h"

EventQueueMutex::EventQueueMutex()
{
}

EventQueueMutex::~EventQueueMutex()
{
}

int EventQueueMutex::PushEvent(Fn&& f)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	if (f)
	{
		m_queue.push(std::forward<Fn>(f));
	}
	return CodeOK;
}

int EventQueueMutex::PopEvent(Fn& f)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	if (m_queue.empty())
	{
		return CodeFalse;
	}

	f = std::move(m_queue.front());
	m_queue.pop();

	return CodeOK;
}

int EventQueueMutex::PopAll(std::function<void(Fn&)> cb)
{
	std::lock_guard<std::mutex> guard(m_mutex);

	while (!m_queue.empty())
	{
		if (cb)
		{
			cb(m_queue.front());
		}
		m_queue.pop();
	}

	return CodeOK;
}


int EventQueueLockFree::PushEvent(Fn&& f)
{
	if (f)
	{
		try
		{
			if (!m_queue.bounded_push(new Fn(f)))
			{
				return CodeFalse;
			}
		}
		catch (...)
		{
			LOG() << "lock free queue push error";
			return CodeFalse;
		}
	}
	return CodeOK;
}

int EventQueueLockFree::PopEvent(Fn& f)
{
	Fn* p = nullptr;
	if (!m_queue.pop(p))
	{
		return CodeFalse;
	}

	if (p)
	{
		f = std::move(*p);
		delete p;
	}

	return CodeOK;
}

int EventQueueLockFree::PopAll(std::function<void(Fn&)> cb)
{
	m_queue.consume_all(
		[&cb](Fn* pF) {
			if (pF )
			{
				if (cb)
				{
					cb(*pF);
				}
				delete pF;
			}
		});
	return CodeOK;
}