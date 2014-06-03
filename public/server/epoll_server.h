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

class EpollServer : public CThread
{
public:
	EpollServer();
	~EpollServer(){};

	int init(int size);

	int notify(int fd, uint64_t data, int iEvent);

	int regProcessor(EventProcessor* pProcessor)
	{
		_event_processor_ptr = pProcessor;
	}

protected:
	virtual void doIt();

private:
	CEpoll _epoll;
	EventProcessor* _event_processor_ptr;
};

#endif /*EPOLL_SERVER_H*/
