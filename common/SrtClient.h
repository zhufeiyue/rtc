#pragma once
#include <future>
#include "Eventloop.h"

class SrtClient
{
public:
	SrtClient(Eventloop&);
	~SrtClient();
	int Connect(std::string address, int16_t port);
	int DisConnect();

protected:
	int ConfigureSrtSocket();

protected:
	void OnConnect(int);
	void OnDisConnect(int);
	void OnError(int);

protected:
	SRTSOCKET m_socket = SRT_INVALID_SOCK;
	Eventloop& m_loop;
};
