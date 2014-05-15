
/* ======================================================
* 
* file:		accept_server.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-15 19:13:12
* 
* ======================================================*/
#ifndef ACCEPT_SERVER_H
#define ACCEPT_SERVER_H 

#include "gc_thread.h"

class EpollServer;

class AcceptServer
{
public:
	AcceptServer(EpollServer* pEpollSvr);
	~AcceptServer(){};

	void init(short port);
	
protected:
	virtual void doIt();

private:
	int fd_;
	EpollServer* epoll_svr_ptr_;
};

#endif /*ACCEPT_SERVER_H*/
