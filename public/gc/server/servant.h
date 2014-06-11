
/* ======================================================
* 
* file:		servant.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-27 21:33:35
* 
* ======================================================*/

#ifndef SERVANT_H
#define SERVANT_H

class SessionManager;
class EpollServer;

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

#endif //SERVANT_H
