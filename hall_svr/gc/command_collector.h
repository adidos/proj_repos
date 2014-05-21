
/* ======================================================
* 
* file:		command_collector.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-19 22:03:22
* 
* ======================================================*/

#ifndef COMMAND_COLLECTOR_H
#define COMMAND_COLLECTOR_H

#include "gc_thread.h"
#include "gc_queue.h"
#include "notify_info.h"

class CmdCollector : public GCThread
{
public:
	CmdCollector(SessionManager* pSession);

	bool collect(NotifyInfo& notify);

protected:
	virtual void doIt() = 0;

private:
	void handleNotify(NotifyInfo& notify);
	
	void handleRead(int seqno);

private:
	SessionManager* session_mgr_ptr_;

	GCQueue<NotifyInfo> notify_array_;

	int queue_empty_wait_ms_;
	int queue_full_wait_ms_;
	
};

#endif //COMMAND_COLLECTOR_H

