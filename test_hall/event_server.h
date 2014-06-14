
/* ======================================================
* 
* file:		event_server.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-13 19:36:43
* 
* ======================================================*/

#ifndef EVENT_SERVER_H
#define EVENT_SERVER_H

#include <stdint.h>
#include <sys/epoll.h>

#include <map>

#include "common/thread.h"

using namespace std;

class EventServer : public CThread
{
public:
	EventServer(int size);
	~EventServer(){};

	int addEvent(int fd, int event, int64_t data);

	int modEvent(int fd, int event, int64_t data);

	int delEvent(int fd, int event, int64_t data);

	//int regHandler(int fd, IEventHandler* pHandler)
	//{
	//	_handler_array[fd] = pHandler;
	//	return 0;
	//}

protected:
	virtual void doIt();

private:
	int ctrl(int fd,int op, int event, int64_t data);

private:
	int _epoll_fd;

	struct epoll_event  _event_array[20480];

	//map<int, IEventHandler*> _handler_array;
};

#endif //EVENT_SERVER_H
