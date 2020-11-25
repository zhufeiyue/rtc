#pragma once
#include "Eventloop.h"

class SrtListener
{
public:
	typedef std::function <void(SRTSOCKET, std::string, int) > FnNewConnect;
	typedef std::function <void(int)> FnListenError;

	SrtListener(Eventloop& loop);
	~SrtListener();
	void SetNewConnectCb(FnNewConnect cb) { m_cbNewConnect = std::move(cb); }
	void SetListenErrorCb(FnListenError cb) { m_cbListenError = std::move(cb); }
	int Listen(std::string address, int port);
	int Stop();

protected:
	void HandleAccept(int ev);

private:
	Eventloop& m_loop;
	SRTSOCKET m_srtSocket = SRT_INVALID_SOCK;
	FnNewConnect m_cbNewConnect;
	FnListenError m_cbListenError;
};
