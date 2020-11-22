#pragma once
#include "../common/Eventloop.h"
#include "../common/SrtListener.h"

class SrtServer
{
public:
	SrtServer(Eventloop&, int workerNum = 0);
	~SrtServer();
	int Stop();
	int Start(std::string, int );

private:
	Eventloop& m_loop;
	int m_iWorkNum = 0;
	std::vector<EventLoopThread*> m_workerPool;
	std::unique_ptr<SrtListener> m_pListener;
};
