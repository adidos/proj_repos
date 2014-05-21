
/* ======================================================
* 
* file:		notify_handler.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-21 11:21:51
* 
* ======================================================*/
#include "notify_handler.h"
#include "gc_logger.h"

NotifyHandler::NotifyHandler(CmdCollector* pCmdCollector)
	: cmd_collector_ptr_(pCmdCollector)
{
}

/**
* brief:
*
* @param 
*/
void NotifyHandler::handle(const NotifyInfo& notify)
{
	if(NT_CLOSE == notify.type || NT_READ == notify.type)
	{
		bool ret = cmd_collector_ptr_->collect(notify);	
		if(!ret)
			LOG4CPLUS_ERROR(GCLogger::ROOT, "put notify to read queue failed!");
	}
	else if(NT_WRITE == notify.type)
	{
		
	}
}

