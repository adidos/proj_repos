
/* ======================================================
* 
* file:		event.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-29 14:18:04
* 
* ======================================================*/

#ifndef GC_EVENT_H
#define GC_EVENT_H

#include <stream>

#include "DataXCmd.h"

enum EVENT_TYPE
{
	EVENT_ERROR,
	EVENT_NEW,
	EVENT_READ,
	EVENT_WRITE,
	EVENT_CLOSE
};

typedef struct event_t
{
	int seqno;				//event source identify
	int type;

	int64_t timestamp; //ms
	
	string dump()
	{
		std::ostringstream oss;
		oss << "{ id: " << id << ", type: " << type << "}";
		return oss.str();
	}
}Event;

typedef struct cmd_task_t
{
	int idx;
	uint32_t seqno;
	int64_t timestamp; //ms
	DataXCmd* pCmd;
}CmdTask;

#endif //GC_EVENT_H
