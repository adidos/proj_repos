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
#include "gc_thread.h"

#define U64(fd, seq) ((uint64_t)fd << 32 | seq )
#define FD(data) ((uint64_t)data >> 32)
#define SEQ(data) (int)((uint64_t)data & 0xFFFFFFFF)

class SessionManager;

class EpollServer : public GCThread
{
public:
	EpollServer(SessionManager* pSessionMgr);
	~EpollServer(){};

	int init(int size);

	int notify(int fd);

protected:
	virtual void doIt();

private:
	int handlerError(uint64_t data);
	int handlerRead(uint64_t data);
	int handlerWrite(uint64_t data);

private:
	SessionManager* session_mgr_ptr_;
	
	GCEpoll epoll_;
};

#endif /*EPOLL_SERVER_H*/
