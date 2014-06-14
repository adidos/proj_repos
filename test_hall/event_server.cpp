
/* ======================================================
* 
* file:		event_server.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-13 19:45:18
* 
* ======================================================*/

#include "event_server.h"
#include "common/logger.h"

#include <string.h>
#include <errno.h>
#include <sys/socket.h>

EventServer::EventServer(int size)
{
	_epoll_fd = epoll_create(size);
}

int EventServer::addEvent(int fd, int event, int64_t data)
{
	return ctrl(fd, EPOLL_CTL_ADD, event, data);
}

int EventServer::modEvent(int fd, int event, int64_t data)
{
	return ctrl(fd, EPOLL_CTL_MOD, event, data);
}

int EventServer::delEvent(int fd, int event,int64_t data)
{
	return ctrl(fd, EPOLL_CTL_DEL, event, data);
}

void EventServer::doIt()
{
	while(true)
	{
		int ret = epoll_wait(_epoll_fd, _event_array, 20480, -1);	
		if(ret < 0)
		{
			if(EINTR == ret)
				continue;	
			
			LOG4CPLUS_ERROR(CLogger::logger, "epoll_wait call error: " << strerror(errno));
			return ;
		}

		
		LOG4CPLUS_DEBUG(CLogger::logger, "epoll_wait result : " << ret);
		for(int i = 0; i < ret; ++i)
		{
			struct epoll_event ev = _event_array[i];


			int fd = ev.data.fd;
			int event = ev.events;
					
			if((EPOLLHUP | EPOLLERR) & event)
			{
				LOG4CPLUS_DEBUG(CLogger::logger, "fd " << fd << " trigger error event"
							<< ", error : " << strerror(errno));
				this->delEvent(fd, EPOLLIN|EPOLLET, fd);
				shutdown(fd, SHUT_RDWR);
				continue;
			}

			if(EPOLLIN & event)
			{
				//pHandler->handleRead(fd);
				char buffer[8192] = {'\0'};
				size_t len = recv(fd, buffer, 8192, 0);
				LOG4CPLUS_DEBUG(CLogger::logger, "fd " << fd << " recv " << len << " bytes");
			}

			if(EPOLLOUT & event)
			{
				LOG4CPLUS_DEBUG(CLogger::logger, "fd " << fd << " trigger write event");
				
				//pHandler->handleWrite(fd);
			}
		}
	}
}

int EventServer::ctrl(int fd, int op , int event,int64_t data)
{
	struct epoll_event ev;
	ev.events = event;
	ev.data.fd = fd;

	return epoll_ctl(_epoll_fd, op, fd, &ev);
}

