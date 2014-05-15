
/* ======================================================
* 
* file:		event_handler.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-11 00:47:04
* 
* ======================================================*/

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

class EpollServer;

struct EventInfo
{
	int event_fd;
	int event_type;
	int event_data;
};

class IEventHandler
{
public:
	virtual void handle(EventInfo event_info) = 0;

	void regEpollSvr(EpollServer* ptr)
	{
		assert(NULL != ptr);
		epoll_svr_ptr_ = ptr;
	}

protected:
	EpollServer* epoll_svr_ptr_;
};

#endif /*EVENT_HANDLER_H*/
