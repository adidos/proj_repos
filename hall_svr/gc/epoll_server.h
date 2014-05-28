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

#include "epoll.h"
#include "thread.h"
#include "servant.h"

#define U64(high, low) ((uint64_t)high << 32 | low)
#define H32(data) (int)((uint64_t)data >> 32)
#define L32(data) (int)((uint64_t)data & 0xFFFFFFFF)

class SessionManager;

class EpollServer : public GCThread
{
public:
	EpollServer(SessionManager* pSessionMgr);
	~EpollServer(){};

	int init(int size);

	int addNewSession(int fd);

	int regServant(Servant* pServant)
	{
		assert(NULL != pServant);
		_servant_ptr = pServant;
	}

	Servant* getServant()
	{
		return _servant_ptr;
	}

protected:
	virtual void doIt();

private:
	int processError(uint64_t data);
	int processRead(uint64_t data);
	int processWrite(uint64_t data);

private:
	SessionManager* _session_mgr_ptr;
	Servant* _servant_ptr;
	
	CEpoll _epoll;
};

#endif /*EPOLL_SERVER_H*/
