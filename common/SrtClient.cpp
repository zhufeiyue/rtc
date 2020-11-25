#include "SrtClient.h"

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "../common/srt/lib/srt_static_d.lib")
#else
#pragma comment(lib, "../common/srt/lib/srt_static.lib")
#endif
#pragma comment(lib, "Ws2_32.lib")
#endif

SrtClient::SrtClient(Eventloop& loop):
	m_loop(loop)
{
}

SrtClient::~SrtClient()
{
}

int SrtClient::Connect(std::string address, int16_t port)
{
	int res;
	sockaddr_in dst_sa = { 0 };
	int dst_sa_len = sizeof(sockaddr_in);

	m_socket = srt_create_socket();
	if (m_socket == SRT_INVALID_SOCK)
	{
		LOG() << "srt_create_socket";
		return CodeFalse;
	}

	if (CodeOK != ConfigureSrtSocket())
	{
		LOG() << "fail to configure srt socket";
		return CodeFalse;
	}

	dst_sa.sin_family = AF_INET;
	dst_sa.sin_port = htons(port);
	inet_pton(AF_INET, address.c_str(), &dst_sa.sin_addr);
	res = SRT_ETIMEOUT;
	res = srt_connect(m_socket, (sockaddr*)&dst_sa, dst_sa_len);
	if (res != SRT_ERRNO::SRT_SUCCESS)
	{
		LOG() << "srt_connect " << srt_getlasterror_str();
	}

	m_loop.AddToEpoll(m_socket, 
		[this](int flag)
		{
			if (flag & SRT_EPOLL_ERR)
			{
				OnError(flag);
				return;
			}
			
			if(flag & SRT_EPOLL_OUT)
			{
				m_loop.AddToEpoll(m_socket, [](int) {}, SRT_EPOLL_IN | SRT_EPOLL_ERR); // update epoll event
				OnConnect(flag);
				return;
			}

			if (flag & SRT_EPOLL_IN)
			{
				char buf[2048] = { 0 };
				srt_recv(m_socket, buf, sizeof(buf));
				LOG() << buf;
				// todo on message
			}
		}, SRT_EPOLL_OUT | SRT_EPOLL_ERR);
	
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

void SrtClient::OnConnect(int)
{
	LOG() << __FUNCTION__;
}

void SrtClient::OnDisConnect(int)
{
	LOG() << __FUNCTION__;
}

void SrtClient::OnError(int)
{
	LOG() << __FUNCTION__;
}