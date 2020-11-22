#pragma once
#include "Eventloop.h"

class SrtListener
{
public:
	typedef std::function < void(SRTSOCKET, std::string, int) > FnNewConnect;

	SrtListener(Eventloop& loop);
	~SrtListener();
	void SetNewConnectCb(FnNewConnect cb) { m_cbNewConnect = std::move(cb); }
	int Listen(std::string address, int port);

protected:
	void HandleAccept(int ev);

private:
	Eventloop& m_loop;
	SRTSOCKET m_srtSocket = SRT_INVALID_SOCK;
	FnNewConnect m_cbNewConnect;
};

