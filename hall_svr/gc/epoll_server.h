
/* ======================================================
* 
* file:		epoll_server.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-15 16:28:28
* 
* ======================================================*/

#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H 

#include "gc_epoll.h"

enum NOTIFY_TYPE
{
	NOTIFY_ADD,
	NOTIFY_DEL
};

class EpollServer : public GCThread
{
public:
	EpollServer(SessionManager* pSessionMgr);
	~EpollServer(){};

	int init(int size);

	int notify(int fd, int type);

protected:
	virtual void doIt();

private:


private:
	SessionManager* session_mgr_ptr_;
	
	GCEpoll epoll_;
};

#endif /*EPOLL_SERVER_H*/
