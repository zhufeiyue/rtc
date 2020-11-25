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