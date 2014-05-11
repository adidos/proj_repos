
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

class GCReactor;

class IEventHandler
{
public:
	virtual void handle(int fd, int event) = 0;

	virtual void handleError(int fd) = 0;

	void regReactor(GCReactor* ptr)
	{
		assert(NULL != ptr);
		reactor_ptr_ = ptr;
	}

protected:
	GCReactor* reactor_ptr_;
};

#endif /*EVENT_HANDLER_H*/
