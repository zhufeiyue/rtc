#pragma once
#include "EventQueueSrt.h"


class Eventloop
{
public:
	Eventloop();
	~Eventloop();

	bool IsRunning() { return m_bRunning; }
	EventQueueLockFree& WorkerEventQueue() { return m_eventQueueWorker; }
	int Run();
	int Quit();

	int AddToEpoll(SRTSOCKET, SrtFn, int);
	int QueueInLoop(Fn&& fn, bool bInWorkQueue=true);


private:
	EventQueueSrt  m_srtEpoll;
	EventQueueLockFree m_eventQueue;
	EventQueueLockFree m_eventQueueWorker;
	/*volatile*/ int m_bRunning = true;
	std::thread::id m_threadID;
};

class EventLoopThread
{
public:
	~EventLoopThread();
	EventLoopThread(Eventloop&);


private:
	Eventloop& m_loop;
};