
/* ======================================================
* 
* file:		fd_reactor.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-15 23:46:10
* 
* ======================================================*/

#include "fd_reactor.h"

#include "transceiver_handle.h"
#include "server/epoll.h"
#include "common/thread_sync.h"
#include "common/function_trace.h"

FDReactor::FDReactor()
{
	_ep.create(1024);
}

FDReactor::~FDReactor()
{

}

void FDReactor::notify(int fd)
{
	LOG4CPLUS_DEBUG(FLogger, "notify reactor fd " << fd 
			<< " have buffer to write out!");

	_ep.mod(fd, fd , EPOLLIN | EPOLLOUT);
}

void FDReactor::doIt()
{
	while(!_terminate)
	{
		int num = _ep.wait(30000);

		if(_terminate) break;

		for(int i = 0; i < num; ++i)
		{
			epoll_event ev = _ep.get(i);
			
			int event = 0;
			int fd = ev.data.fd;

			if(ev.events & EPOLLIN)
			{
				LOG4CPLUS_DEBUG(FLogger, "epollin occure!");

				event = TransceiverHandle::R;				
			}

			if(ev.events & EPOLLOUT)
			{
				LOG4CPLUS_DEBUG(FLogger, "epollout occure!");

				event = event | TransceiverHandle::W;				
			}

			LOG4CPLUS_DEBUG(FLogger, "fd " << fd << " occure event : " << event);

			handle(fd, event);
		}
	}

}

void FDReactor::handle(int fd, int events)
{
	TransceiverHandle* ptr = NULL;
	
	{
		CScopeGuard guard(_mutex);
		map<int, TransceiverHandle*>::iterator iter = _handles.find(fd);
		if(_handles.end() == iter)
		{
			LOG4CPLUS_ERROR(FLogger, "couldn't find fd " << fd  <<" transceiver handle!");
	
			_ep.del(fd, fd, 0);
			return;
		}
		
		ptr = iter->second;
	}

	if(events & (EPOLLERR | EPOLLHUP))
	{
		ptr->handleExcept(fd);
	}
	else if(0 != events)
	{
		ptr->handle(fd, events);
	}
}

void FDReactor::regHandle(int fd, uint32_t event, TransceiverHandle* pHandle)
{
	{
		CScopeGuard guard(_mutex);

		_handles[fd] = pHandle;
	}
	
	_ep.del(fd, fd, 0);

	_ep.add(fd, fd, event);

	LOG4CPLUS_DEBUG(FLogger, fd << " add into reactor!");
}

void FDReactor::unregHandle(int fd, uint32_t event, TransceiverHandle* pHandle)
{
	{
		CScopeGuard guard(_mutex);
	
		_handles.erase(_handles.find(fd));
	}

	_ep.del(fd, fd, 0);

	LOG4CPLUS_DEBUG(FLogger, fd << " erase from reactor !");
}
