#include "SrtServer.h"

SrtConnection::SrtConnection(SRTSOCKET u):
	m_clientSocket(u)
{
}

int SrtConnection::Clear()
{
	return CodeOK;
}

int SrtServer::Create()
{
	sockaddr_in sa = { 0 };
	int saLen = sizeof(sa);
	int res(0);
	int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
	auto u = srt_create_socket();
	if (u == SRT_INVALID_SOCK)
	{
		LOG() << "srt_create socket " << u;
		return CodeFalse;
	}
	sa.sin_family = AF_INET;
	sa.sin_port = htons(8000);
	inet_pton(AF_INET, "0.0.0.0", &sa.sin_addr);
	res = srt_bind(u, (sockaddr*)&sa, saLen);
	if (res != SRT_ERRNO::SRT_SUCCESS)
	{
		LOG() << srt_getlasterror_str();
		return CodeFalse;
	}
	res = srt_listen(u, 5);
	if (res != SRT_ERRNO::SRT_SUCCESS)
	{
		LOG() << srt_getlasterror_str();
		return CodeFalse;
	}

	m_socket = u;
	m_srt_epoll = srt_epoll_create();

	res = srt_epoll_add_usock(m_srt_epoll, u, &events);
	if (res != SRT_ERRNO::SRT_SUCCESS)
	{
		LOG() << srt_getlasterror_str();
		srt_close(m_socket);
		srt_epoll_release(m_srt_epoll);
		m_srt_epoll = 0;
		m_socket = 0;
		return CodeFalse;
	}

	m_bServerLoopRunning = true;
	m_fuServer = std::async(std::launch::async, [this]() 
		{
			return this->ServerLoop();
		});
	return CodeOK;
}

int SrtServer::Destroy()
{
	m_bServerLoopRunning = false;
	if (m_fuServer.valid())
	{
		m_fuServer.get();
	}

	return CodeOK;
}

SrtConnection* SrtServer::CreateConnectionHandler(SRTSOCKET u, int)
{
	auto pHandler = new SrtConnection(u);
	return pHandler;
}

int SrtServer::ServerLoop()
{
	if (m_srt_epoll == 0)
	{
		LOG() << "invalid srt epoll";
		return CodeFalse;
	}

	int res(0), i(0), size(0);
	const int kFDSize = 10;
	SRT_EPOLL_EVENT srtSocketFD[kFDSize];
	SRT_SOCKSTATUS status = SRTS_INIT;
	SRTSOCKET u, cu;
	sockaddr_in csa;
	int csa_len = sizeof(csa);
	int cevents = SRT_EPOLL_IN | SRT_EPOLL_ERR;
	bool bSync = false;
	decltype(m_clients.begin()) iter;
	int ue;

	while (m_bServerLoopRunning)
	{
		size = srt_epoll_uwait(m_srt_epoll, srtSocketFD, kFDSize, 200);
		if (size <= 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		for (i = 0; i < size && i < kFDSize; ++i)
		{
			u = srtSocketFD[i].fd;
			ue = srtSocketFD[i].events;
			if (u == SRT_INVALID_SOCK)
			{
				continue;
			}

			if (u == m_socket)
			{
				if (ue & SRT_EPOLL_ERR)
				{
					LOG() << "server socket report error ";
					m_bServerLoopRunning = false;
					continue;
				}

				if (ue & SRT_EPOLL_IN)
				{
					LOG() << "new connection";
					cu = srt_accept(m_socket, (sockaddr*)&csa, &csa_len);
					res = srt_setsockflag(cu, SRTO_SNDSYN, &bSync, sizeof(bSync));
					res = srt_setsockflag(cu, SRTO_RCVSYN, &bSync, sizeof(bSync));
					res = srt_epoll_add_usock(m_srt_epoll, cu, &cevents);
					if (res != SRT_ERRNO::SRT_SUCCESS)
					{
						LOG() << "srt_epoll_add_usock " << srt_getlasterror_str();
						srt_close(cu);
					}
					else
					{
						srt_send(cu, "123", 3);
						srt_send(cu, "456", 3);

						auto pHandler = CreateConnectionHandler(cu, 0);
						if (pHandler)
						{
							m_clients.insert(std::make_pair(cu, pHandler));
						}
					}
				}
				continue;
			}

			if (ue & SRT_EPOLL_ERR)
			{
				status = srt_getsockstate(u);
				res = srt_epoll_remove_usock(m_srt_epoll, u);
				LOG() << srt_getlasterror_str() << " lost connection " << status;
				iter = m_clients.find(u);
				if (iter != m_clients.end())
				{
					delete iter->second;
					m_clients.erase(iter);
				}
				continue;
			}

			if (ue & SRT_EPOLL_IN)
			{
				char buf[1500] = { 0 };
				srt_recv(u, buf, 1500);
				LOG() << buf;
			}
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	for (iter = m_clients.begin(); iter != m_clients.end(); ++iter)
	{
		if (iter->second)
		{
			iter->second->Clear();
			delete iter->second;
		}
	}
	m_clients.clear();

	srt_epoll_clear_usocks(m_srt_epoll);
	srt_epoll_release(m_srt_epoll);
	srt_close(m_socket);

	return CodeOK;
}