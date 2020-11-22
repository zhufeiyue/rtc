#include "SrtListener.h"

SrtListener::SrtListener(Eventloop& loop):
	m_loop(loop)
{
}

SrtListener::~SrtListener()
{
	srt_close(m_srtSocket);
}

int SrtListener::Listen(std::string address, int port)
{
	sockaddr_in sa = { 0 };
	int saLen = sizeof(sa);
	int res;
	bool bSync = false;

	m_srtSocket = srt_create_socket();
	if (m_srtSocket == SRT_INVALID_SOCK)
	{
		LOG() << __FUNCTION__ << " srt_create_socket SRT_INVALID_SOCK";
		return CodeFalse;
	}
	res = srt_setsockflag(m_srtSocket, SRTO_SNDSYN, &bSync, sizeof(bSync));
	res = srt_setsockflag(m_srtSocket, SRTO_RCVSYN, &bSync, sizeof(bSync));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	inet_pton(AF_INET, address.c_str(), &sa.sin_addr);
	res = srt_bind(m_srtSocket, (sockaddr*)&sa, saLen);
	if (res != SRT_ERRNO::SRT_SUCCESS)
	{
		LOG() << __FUNCTION__ << srt_getlasterror_str();
		return CodeFalse;
	}
	res = srt_listen(m_srtSocket, 5);
	if (res != SRT_ERRNO::SRT_SUCCESS)
	{
		LOG() << __FUNCTION__ << srt_getlasterror_str();
		return CodeFalse;
	}

	res = m_loop.AddToEpoll(m_srtSocket, 
		std::bind(&SrtListener::HandleAccept, this, std::placeholders::_1), 
		SRT_EPOLL_IN | SRT_EPOLL_ERR);

	return res;
}

void SrtListener::HandleAccept(int ev)
{
	SRTSOCKET u;
	sockaddr_in sa;
	int salen = sizeof(sa);

	u = srt_accept(m_srtSocket, (sockaddr*)&sa, &salen);
	if (u == SRT_INVALID_SOCK)
	{
		LOG() << __FUNCTION__ << " srt_accept " << srt_getlasterror_str();
		return;
	}

	auto port = ntohs(sa.sin_port);
	char buf[32] = { 0 };
	inet_ntop(AF_INET, (void*)&sa, buf, sizeof(buf));

	if (m_cbNewConnect)
	{
		m_cbNewConnect(u, std::string(buf), port);
	}
	else
	{
		srt_close(u);
	}
}