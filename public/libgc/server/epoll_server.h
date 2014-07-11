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

#include <map>

#include "epoll.h"
#include "common/thread.h"
#include "common/thread_sync.h"

#define U64(high, low) ((uint64_t)high << 32 | low)
#define H32(data) (int)((uint64_t)data >> 32)
#define L32(data) (int)((uint64_t)data & 0xFFFFFFFF)

#define EPOLL_WAIT_TIME  1000 * 30
#define CHECK_INTERVAL 60
#define INACTIVE_INTERVAL 5 * 60

class SessionManager;
class EventProcessor;

class EpollServer : public CThread
{
public:
	EpollServer();
	~EpollServer(){};

	int init(int size);

	int notify(int fd, uint64_t data, int iEvent);

	void regProcessor(EventProcessor* pProcessor)
	{
		_event_processor_ptr = pProcessor;
	}

protected:
	virtual void doIt();

private:
	void clean_inactive_fd();

	bool is_inactive(pair<int64_t,time_t> value);

private:
	CEpoll _epoll;
	EventProcessor* _event_processor_ptr;

	CMutex _mutex;
	map<int64_t, time_t> _fd_array;
};

#endif /*EPOLL_SERVER_H*/
