#include "SrtServer.h"

SrtServer::SrtServer(Eventloop& loop, int workerNum)
	:m_loop(loop),
	m_iWorkNum(workerNum)
{
	if (m_iWorkNum < 0)
		m_iWorkNum = 0;
}

SrtServer::~SrtServer()
{
}

int SrtServer::Stop()
{
	return CodeOK;
}

int SrtServer::Start(std::string address, int port)
{
	if (m_pListener)
	{
		return CodeFalse;
	}
	m_pListener.reset(new SrtListener(m_loop));
	if (CodeOK != m_pListener->Listen(address, port))
	{
		LOG() << "listen " << address << ' ' << port << " fail";
		return CodeFalse;
	}

	for (int i = 0; i < m_iWorkNum; ++i)
	{
		m_workerPool.emplace_back(new EventLoopThread(m_loop));
	}

	return CodeOK;
}