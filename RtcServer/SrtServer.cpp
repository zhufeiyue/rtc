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
	if (m_pListener)
	{
		m_pListener->Stop();
	}

	for (int i = 0; i < m_iWorkNum; ++i)
	{
		auto pWorkThread = m_workerPool[i];
		pWorkThread->Stop();
		delete pWorkThread;
	}
	m_workerPool.clear();

	return CodeOK;
}

int SrtServer::Start(std::string address, int port)
{
	if (m_pListener)
	{
		return CodeFalse;
	}

	m_pListener.reset(new SrtListener(m_loop));
	m_pListener->SetNewConnectCb(std::bind(&SrtServer::OnNewConnect, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	m_pListener->SetListenErrorCb(std::bind(&SrtServer::OnError, this, std::placeholders::_1));
	if (CodeOK != m_pListener->Listen(address, port))
	{
		LOG() << "listen " << address << ' ' << port << " fail";
		return CodeFalse;
	}

	for (int i = 0; i < m_iWorkNum; ++i)
	{
		auto pWorkThread = new EventLoopThread(m_loop);
		pWorkThread->Start();
		m_workerPool.emplace_back(pWorkThread);
	}

	return CodeOK;
}

void SrtServer::OnNewConnect(SRTSOCKET u, std::string address, int port)
{
	//srt_close(u);
	//srt_send(u, "12345678", 5);
	//srt_send(u, "etyhk,olp;o", 5);
	//srt_send(u, "23434rwr", 5);
	//srt_send(u, "12erfwer345678", 5);
	//srt_send(u, "12345erefer678", 5);
	//srt_send(u, "rrrrrer", 5);

	if (!m_cbConnection)
	{
		srt_close(u);
		return;
	}

	auto pConn = std::make_shared<SrtConn>(m_loop, u, std::move(address), port);
	auto p = pConn->shared_from_this();
	m_cbConnection(std::move(pConn));
}

void SrtServer::OnError(int)
{
	LOG() << __FUNCTION__;
}