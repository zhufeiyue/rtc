#pragma once
#include "../common/Eventloop.h"

class SrtConn : public std::enable_shared_from_this<SrtConn>
{
public:
	SrtConn(Eventloop&, SRTSOCKET, std::string, int );
	~SrtConn();

private:
	Eventloop& m_loop;
	std::string m_strAddress;
	int m_iPort = 0;
	SRTSOCKET m_socket = SRT_INVALID_SOCK;
};

typedef std::shared_ptr<SrtConn> SrtConnPtr;