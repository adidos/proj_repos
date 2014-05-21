
/* ======================================================
* 
* file:		notify_handler.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-21 11:07:45
* 
* ======================================================*/

#ifndef NOTIFY_HANDLER_H
#define NOTIFY_HANDLER_H

#include "notify_info.h"
#include "command_collector.h"

class NotifyHandler
{
public:
	NotifyHandler(CmdCollector* pCmdCollector);
	~NotifyHandler(){};

	void handle(const NotifyInfo& notify);
	void handleRead(const NotifyInfo& notify);
	void handleWrite(const NotifyInfo& notify);
	void handleClose(const NotifyInfo& notify);

private:
	CmdCollector* cmd_collector_ptr_;
	
};

#endif //NOTIFY_HANDLER_H
