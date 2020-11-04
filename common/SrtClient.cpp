#include "SrtClient.h"

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "../common/srt/lib/srt_static_d.lib")
#else
#pragma comment(lib, "../common/srt/lib/srt_static.lib")
#endif
#pragma comment(lib, "Ws2_32.lib")
#endif

SrtClient::SrtClient()
{
}

SrtClient::~SrtClient()
{
	if (m_fuClient.valid())
	{
		m_fuClient.get();
	}
}

int SrtClient::Connect(std::string address, int16_t port)
{
	SRTSOCKET u = srt_create_socket();
	int res;
	sockaddr_in dst_sa = { 0 };
	int dst_sa_len = sizeof(sockaddr_in);
	int events =  SRT_EPOLL_OUT | SRT_EPOLL_ERR;

	if (u == SRT_INVALID_SOCK)
	{
		LOG() << "srt_create_socket";
		return CodeFalse;
	}
	m_socket = u;
	if (CodeOK != ConfigureSrtSocket())
	{
		LOG() << "fail to configure srt socket";
		return CodeFalse;
	}

	m_srt_epoll = srt_epoll_create();
	//srt_epoll_set(m_srt_epoll, SRT_EPOLL_ENABLE_EMPTY);
	res = srt_epoll_add_usock(m_srt_epoll, u, &events);
	if (res != SRT_ERRNO::SRT_SUCCESS)
	{
		srt_epoll_release(m_srt_epoll);
		srt_close(m_socket);
		m_socket = SRT_INVALID_SOCK;
		m_srt_epoll = 0;
		return CodeFalse;
	}

	m_bClientLoopRunning = true;
	m_fuClient = std::async(std::launch::async, [this]()
		{
			return ClientLoop();
		});

	dst_sa.sin_family = AF_INET;
	dst_sa.sin_port = htons(port);
	inet_pton(AF_INET, address.c_str(), &dst_sa.sin_addr);
	res = SRT_ETIMEOUT;
	res = srt_connect(u, (sockaddr*)&dst_sa, dst_sa_len);
	if (res != SRT_ERRNO::SRT_SUCCESS)
	{
		LOG() << "srt_connect " << srt_getlasterror_str();
	}
	
	return CodeOK;
}

int SrtClient::DisConnect()
{
	if (m_socket != SRT_INVALID_SOCK)
	{
		srt_close(m_socket);
		m_socket = SRT_INVALID_SOCK;
	}

	return CodeOK;
}

int SrtClient::ConfigureSrtSocket()
{
	bool bSync = false;
	int res = 0;

	if (m_socket == SRT_INVALID_SOCK)
	{
		return CodeFalse;
	}

	res = srt_setsockflag(m_socket, SRTO_SNDSYN, &bSync, sizeof(bSync));
	res = srt_setsockflag(m_socket, SRTO_RCVSYN, &bSync, sizeof(bSync));
	if (res != SRT_ERRNO::SRT_SUCCESS)
	{
		LOG() << srt_getlasterror_str();
		return CodeFalse;
	}

	int mode = SRTT_LIVE;
	res = srt_setsockflag(m_socket, SRTO_TRANSTYPE, &mode, sizeof(mode));

	return CodeOK;
}

int SrtClient::ClientLoop()
{
	int res(0), size(0), temp(0);
	SRT_EPOLL_EVENT socketFD[2];
	SRT_SOCKSTATUS state;
	SRTSOCKET u;
	int ue;

	while (m_bClientLoopRunning)
	{
		size = srt_epoll_uwait(m_srt_epoll, socketFD, 2, 200);
		if (size == 0)
		{
			if (temp++ > 30) 
			{
				DisConnect();
			}
			continue;
		}
		else if (size < 0)
		{
			OnDisConnect(0);
			continue;
		}

		u = socketFD[0].fd;
		ue = socketFD[0].events;

		if (ue & SRT_EPOLL_ERR)
		{
			state = srt_getsockstate(u);
			LOG() << "socket state " << state;
			OnDisConnect(0);
			break;
		}

		if (ue & SRT_EPOLL_IN)
		{
			char buf[1500] = { 0 };
			srt_recv(u, buf, 1500);
			LOG() << buf;

			continue;
		}

		if (ue & SRT_EPOLL_OUT)
		{
			int newEvents = SRT_EPOLL_IN | SRT_EPOLL_ERR;
			res = srt_epoll_update_usock(m_srt_epoll, u, &newEvents);
			if (res != SRT_ERRNO::SRT_SUCCESS)
			{
				LOG() << "srt_epoll_update_usock " << srt_getlasterror_str();
			}

			OnConnect(0);
		}
	}

	srt_epoll_clear_usocks(m_srt_epoll);
	srt_epoll_release(m_srt_epoll);
	m_srt_epoll = 0;

	if (m_socket != SRT_INVALID_SOCK)
	{
		srt_close(m_socket);
		m_socket = SRT_INVALID_SOCK;
	}

	return CodeOK;
}

void SrtClient::OnConnect(int)
{
	LOG() << "on connect";
	srt_send(m_socket, "abc", 3);
}

void SrtClient::OnDisConnect(int)
{
	m_bClientLoopRunning = false;
	LOG() << " on disconnect";
}