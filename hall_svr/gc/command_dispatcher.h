
/* ======================================================
* 
* file:		command_dispatcher.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-19 20:41:17
* 
* ======================================================*/

#ifndef COMMAND_DISPATCHER_H
#define COMMAND_DISPATCHER_H

#include "notify_info.h"
#include "gc_queue.h"
#include "gc_thread.h"

class CmdDispatcher : public GCThread
{
public:
	CmdDispatcher();


};

#endif //COMMAND_DISPATCHER_H
