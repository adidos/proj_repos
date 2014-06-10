
/* ======================================================
* 
* file:		servant.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-27 21:33:35
* 
* ======================================================*/

#ifndef GC_SERVANT_H
#define GC_SERVANT_H

#include "application.h"
#include "epoll_server.h"
#include "session_manager.h"

#include "common/thread.h"

class Servant : public CThread
{
public:
	Servant(SessionManager* pSessMgr);
	~Servant();
	
	int init();

	int startService();

	int waitForStop();

	EpollServer* getEpollServer()
	{
		return _epoll_svr_ptr;
	}

protected:
	virtual void doIt();

private:
	int initAcceptor();

	int newConnection(int client);

private:
	int _acceptor;
	EpollServer* _epoll_svr_ptr;
	SessionManager* _session_mgr_ptr;
	
};

#endif //GC_SERVANT_H
