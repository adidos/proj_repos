
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

FDReactor::FDReactor()
	:_terminate(false)
{
	_ep.create(1024);
}

FDReactor::~FDReactor()
{

}

void FDReactor::notify(int fd)
{
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
			
			int fd = ev.data.fd;

			int event = (ev.events & EPOLLIN) ? TransceiverHandle::R : 0;
			
			event = event | (ev.events & EPOLLOUT) ? TransceiverHandle::W : 0;

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

void FDReactor::regHandle(int fd, uint32_t event, TransceiverHandle* pHanndle)
{
	CScopeGuard guard(_mutex);

	handle->setFDRactor(this);

	_handles[fd] = pHanndle;
	
	_ep.del(fd, fd, 0);

	_ep.add(fd, fd, event);
}

void FDReactor::unregHandle(int fd, uint32_t event, TransceiverHandle* pHanndle)
{
	CScopeGuard guard(_mutex);

	_ep.del(fd, fd, 0);
	
	_handles.erase(_handles.find(fd));

}
