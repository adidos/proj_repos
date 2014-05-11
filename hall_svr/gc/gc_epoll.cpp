
/* ======================================================
* 
* file:		gc_epoll.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-10 18:40:37
* 
* ======================================================*/

#include "gc_epoll.h"

#include <unistd.h>

GCEpoll::GCEpoll(bool bEt)
{
	epoll_fd_ = -1;
	max_connections_ = 65535;
	event_array_ = NULL;
	et_ = bEt;
}

GCEpoll::~GCEpoll()
{
	if(NULL != event_array_)
	{
		delete []event_array_;
		event_array_ = NULL;
	}

	if(epoll_fd_ > 0)
	{
		close(epoll_fd_);
	}

}

/**
* brief:
*
* @param max_connections 
*
* @returns return the value of the epoll desciptor for check 
*/
int GCEpoll::create(int max_connections)
{
	max_connections_ = max_connections;
	epoll_fd_ = epoll_create(max_connections);
	
	if(NULL != event_array_)
	{
		delete []event_array_;
		event_array_ = NULL;
	}

	event_array_ = new epoll_event[max_connections];

	return epoll_fd_;
}

/**
* brief:
*
* @param fd
* @param data
* @param event
* @param op
*/
void GCEpoll::ctrl(int fd, void* data, uint32_t event, int op)
{
	struct epoll_event ev;
	ev.data.ptr = data;

	if(et_)
		ev.events = event | EPOLLET;
	else
		ev.events = event ;

	epoll_ctl(epoll_fd_, op, fd, &ev);
}


/**
* brief:
*
* @param fd
* @param data
* @param event
*
* @returns   
*/
int GCEpoll::add(int fd, void* data, uint32_t event)
{
	ctrl(fd, data, event, EPOLL_CTL_ADD); 
	return 0;
}

/**
* brief:
*
* @param fd
* @param data
* @param event
*
* @returns   
*/
int GCEpoll::mod(int fd, void* data, uint32_t event)
{
	ctrl(fd, data, event, EPOLL_CTL_MOD); 

	return 0;
}

/**
* brief:
*
* @param fd
* @param data
* @param event
*
* @returns   
*/
int GCEpoll::del(int fd, void* data, uint32_t event)
{
	ctrl(fd, data, event, EPOLL_CTL_DEL); 

	return 0;
}

/**
* brief:
*
* @param millsecond
*
* @returns   
*/
int GCEpoll::wait(int millsecond)
{
	int ret = epoll_wait(epoll_fd_, event_array_, max_connections_, millsecond);
	return ret;
}










