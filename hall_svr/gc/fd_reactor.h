
/* ======================================================
* 
* file:		gc_reactor.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-10 21:31:53
* 
* ======================================================*/
#ifndef GC_REACTOR_H
#define GC_REACTOR_H

#include "gc_thread.h"

class GCReactor : public GCThread
{
public:
	GCReactor();
	~GCReactor();

	
protected:
	virtual void doIt();

private:
	GCEpoll epoll_;
	map<int, >	


};

#endif /*GC_REACTOR_H*/
