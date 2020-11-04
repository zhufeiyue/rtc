#pragma once
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <boost/lockfree/queue.hpp>
#include "common.h"

typedef std::function<void()> Fn;

class EventQueue
{
public:
	EventQueue() {}
	virtual ~EventQueue() {}
	virtual int PushEvent(Fn&&) = 0;
	virtual int PopEvent(Fn&) = 0;
	virtual int PopAll(std::function<void(Fn&)>) = 0;
};

class EventQueueMutex : public EventQueue
{
public:
	EventQueueMutex();
	~EventQueueMutex();
	int PushEvent(Fn&&) override;
	int PopEvent(Fn&) override;
	int PopAll(std::function<void(Fn&)>) override;
private:
	std::mutex m_mutex;
	std::queue<Fn> m_queue;
};

class EventQueueLockFree : public EventQueue
{
public:
	int PushEvent(Fn&&) override;
	int PopEvent(Fn&) override;
	int PopAll(std::function<void(Fn&)>) override;
private:
	boost::lockfree::queue<Fn*, boost::lockfree::capacity<4096>, boost::lockfree::fixed_sized<true>> m_queue;
};