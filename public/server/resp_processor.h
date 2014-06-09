
/* ======================================================
* 
* file:		resp_processor.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-09 11:36:07
* 
* ======================================================*/

#ifndef RESP_PROCESSOR_H
#define RESP_PROCESSOR_H

#include "queue.h"
#include "thread.h"

class RespProcessor
{
public:
	RespProcessor();
	~RespProcessor();

	int putResponse(CmdTask& resp);

protected:
	virtual void doIt();

private:
	EpollServer* _epoll_svr_ptr;
	Queue<CmdTask> _resp_array;
	



}

#endif //RESP_PROCESSOR_H
