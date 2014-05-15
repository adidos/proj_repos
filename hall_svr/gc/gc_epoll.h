
/* ======================================================
* 
* file:		gc_epoll.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-10 18:39:34
* 
* ======================================================*/

#ifndef EPOLL_WRAPP_H
#define EPOLL_WRAPP_H

#include <sys/epoll.h>

#include <stdint.h>
#include <cassert>
#include <cstddef>

class GCEpoll
{
public:
	GCEpoll(bool bEt = true);

	~GCEpoll();

	int create(int max_connections);

	int add(int fd, uint32_t data, uint32_t event);
	
	int mod(int fd, uint32_t data, uint32_t event);

	int del(int fd, uint32_t data, uint32_t event);

	int wait(int millsecond);

	struct epoll_event& get(int i)
	{
		assert(NULL != event_array_);
		return event_array_[i];
	}

private:
	void ctrl(int fd, uint32_t data, uint32_t event, int op);

private:
	int epoll_fd_;
	
	int max_connections_;

	struct epoll_event* event_array_;

	bool et_;
};

#endif /*EPOLL_PRI_H*/
