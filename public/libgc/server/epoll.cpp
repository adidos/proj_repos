
/* ======================================================
* 
* file:		epoll.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-27 22:02:45
* 
* ======================================================*/

#include "epoll.h"

#include <unistd.h>

CEpoll::CEpoll(bool bEt)
{
	_epoll_fd = -1;
	_max_connections = 65535;
	_event_array = NULL;
	_et = bEt;
}

CEpoll::~CEpoll()
{
	if(NULL != _event_array)
	{
		delete []_event_array;
		_event_array = NULL;
	}

	if(_epoll_fd > 0)
	{
		close(_epoll_fd);
	}

}

/**
* brief:
*
* @param max_connections 
*
* @returns return the value of the epoll desciptor for check 
*/
int CEpoll::create(int max_connections)
{
	_max_connections = max_connections;
	_epoll_fd = epoll_create(max_connections);
	
	if(NULL != _event_array)
	{
		delete []_event_array;
		_event_array = NULL;
	}

	_event_array = new epoll_event[max_connections];

	return _epoll_fd;
}

/**
* brief:
*
* @param fd
* @param data
* @param event
* @param op
*/
void CEpoll::ctrl(int fd, uint64_t data, uint32_t event, int op)
{
	struct epoll_event ev;
	ev.data.u64 = data;

	if(_et)
		ev.events = event | EPOLLET;
	else
		ev.events = event ;

	epoll_ctl(_epoll_fd, op, fd, &ev);
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
int CEpoll::add(int fd, uint64_t data, uint32_t event)
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
int CEpoll::mod(int fd, uint64_t data, uint32_t event)
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
int CEpoll::del(int fd, uint64_t data, uint32_t event)
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
int CEpoll::wait(int millsecond)
{
	int ret = epoll_wait(_epoll_fd, _event_array, _max_connections, millsecond);
	return ret;
}

