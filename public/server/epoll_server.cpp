
/* ======================================================
* 
* file:		_epollserver.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-04 19:53:03
* 
* ======================================================*/

#include "_epollserver.h"
#include "logger.h"
#include "session_manager.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

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
			LOG4CPLUS_ERROR(CLogger::logger, "notify unknow event, " << iEvent);
	}
}

/**
* brief:
*
* @returns   
*/
void EpollServer::doIt()
{
	while(!teminate_)
	{
		//block until event occure
		int ret = _epoll.wait(-1);
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;

			LOG4CPLUS_ERROR(CLogger::logger, "epoll server error, exit run loop...");
			break;
		}
		
		LOG4CPLUS_TRACE(CLogger::logger, "epoll wait return value " << ret);
		for(int i = 0; i < ret; ++i)
		{
			struct _epollevent ev = _epoll->get(i);

			CEvent event;
			event.id = H32(ev.data.u64)
			event.timestamp = current_time_usec();
			if(ev.events & (EPOLLHUP | EPOLLERR))
			{
				LOG4CPLUS_TRACE(CLogger::logger, "epoll error, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));
				event.type = EVENT_CLOSE;
			}
			else if(ev.events & EPOLLIN)
			{
				LOG4CPLUS_TRACE(CLogger::logger, "epoll readable, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));
				event.type = EVENT_READ;
			}
			else if(ev.events & EPOLLOUT)
			{
				LOG4CPLUS_TRACE(CLogger::logger, "epoll writeable, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));
				event.type = EVENT_WRITE;
			}

			_event_processor_ptr->addEvent(event)
		}
	}

}

