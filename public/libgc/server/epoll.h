
/* ======================================================
* 
* file:		epoll.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-27 22:01:53
* 
* ======================================================*/

#ifndef EPOLL_WRAPP_H
#define EPOLL_WRAPP_H

#include <sys/epoll.h>

#include <stdint.h>
#include <cassert>
#include <cstddef>

class CEpoll
{
public:
	CEpoll(bool bEt = true);

	~CEpoll();

	int create(int max_connections);

	int add(int fd, uint64_t data, uint32_t event);
	
	int mod(int fd, uint64_t data, uint32_t event);

	int del(int fd, uint64_t data, uint32_t event);

	int wait(int millsecond);

	struct epoll_event& get(int i)
	{
		assert(NULL != _event_array);
		return _event_array[i];
	}

private:
	void ctrl(int fd, uint64_t data, uint32_t event, int op);

private:
	int _epoll_fd;
	
	int _max_connections;

	struct epoll_event* _event_array;

	bool _et;
};

#endif /*EPOLL_PRI_H*/
