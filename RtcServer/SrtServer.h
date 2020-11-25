#pragma once
#include "../common/Eventloop.h"
#include "../common/SrtListener.h"
#include "SrtConn.h"

class SrtServer
{
public:
	typedef std::function< void(SrtConnPtr)> ConnectionCb;

public:
	SrtServer(Eventloop&, int workerNum = 0);
	~SrtServer();
	int Stop();
	int Start(std::string, int );
	void SetConnectionCb(ConnectionCb cb) { m_cbConnection = std::move(cb); }

protected:
	void OnNewConnect(SRTSOCKET, std::string, int);
	void OnError(int);

private:
	Eventloop& m_loop;
	int m_iWorkNum = 0;
	std::vector<EventLoopThread*> m_workerPool;
	std::unique_ptr<SrtListener> m_pListener;
	ConnectionCb m_cbConnection;
};
