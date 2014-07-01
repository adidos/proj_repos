
/* ======================================================
* 
* file:		_epollserver.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-04 19:53:03
* 
* ======================================================*/

#include "epoll_server.h"
#include "session_manager.h"
#include "event.h"
#include "event_processor.h"
#include "common/logger.h"
#include "common/utility.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include <utility>

using namespace std;

EpollServer::EpollServer()
	:_event_processor_ptr(NULL)
{
}

/**
* brief:
*
* @param size
*
* @returns   
*/
int EpollServer::init(int size)
{
	_epoll.create(size);

	return 0;
}

int EpollServer::notify(int fd, uint64_t data, int iEvent)
{
	LOG4CPLUS_DEBUG(FLogger, "epoll notify: " << fd << "|" 
			<< data<< "|" << iEvent );
	switch(iEvent)
	{
		case EVENT_NEW:
			_epoll.add(fd, data, EPOLLET | EPOLLIN);	
			break;

		case EVENT_READ:
			_epoll.mod(fd, data, EPOLLET | EPOLLIN);
			break;

		case EVENT_WRITE:
			_epoll.mod(fd, data, EPOLLET | EPOLLOUT);
			break;

		case EVENT_CLOSE:
		case EVENT_ERROR:
			_epoll.del(fd, data, EPOLLET | EPOLLIN | EPOLLOUT);
			break;

		default:
			LOG4CPLUS_ERROR(FLogger, "notify unknow event, " << iEvent);
	}

	return 0;
}

/**
* brief:
*
* @returns   
*/
void EpollServer::doIt()
{
	LOG4CPLUS_DEBUG(FLogger, "event processor start work!");

	while(true)
	{
		//block until event occure
		int ret = _epoll.wait(-1);
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;

			LOG4CPLUS_ERROR(FLogger, "epoll server error, exit run loop...");
			break;
		}
		
		LOG4CPLUS_TRACE(FLogger, "epoll wait return value " << ret);
		for(int i = 0; i < ret; ++i)
		{
			struct epoll_event ev = _epoll.get(i);

			Event event = {'\0'};
			event.seqno = H32(ev.data.u64);
			event.timestamp = current_time_usec();
			if(ev.events & (EPOLLHUP | EPOLLERR))
			{
				LOG4CPLUS_TRACE(FLogger, "epoll error, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));
				event.type = EVENT_CLOSE;
			}
			else if(ev.events & EPOLLIN)
			{
				LOG4CPLUS_TRACE(FLogger, "epoll readable, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));
				event.type = EVENT_READ;
			}
			else if(ev.events & EPOLLOUT)
			{
				LOG4CPLUS_TRACE(FLogger, "epoll writeable, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));
				event.type = EVENT_WRITE;
			}

			_event_processor_ptr->addEvent(event);
		}
	}
}

