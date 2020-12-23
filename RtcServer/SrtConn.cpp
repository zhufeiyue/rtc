#include "SrtConn.h"

SrtConn::SrtConn(Eventloop& loop, SRTSOCKET u, std::string address, int port):
	m_loop(loop),
	m_strAddress(std::move(address)),
	m_iPort(port),
	m_socket(u)
{
}

SrtConn::~SrtConn()
{
}

int SrtConn::Init()
{
	m_loop.AddToEpoll(m_socket, [this](int flag)
		{

		}, SRT_EPOLL_IN | SRT_EPOLL_ERR);
	return CodeOK;
}

int SrtConn::Destroy()
{
	return CodeOK;
}