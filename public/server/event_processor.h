
/* ======================================================
* 
* file:		event_processor.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-29 14:24:48
* 
* ======================================================*/

#ifndef EVENT_PROCESSOR_H
#define EVENT_PROCESSOR_H

#include "event.h"
#include "queue.h"
#include "thread.h"
#include "DataXCmd.h"

#define QUEUE_SIZE 1024000
#define QUEUE_WAIT_MS 100

class EventProcessor : public CThread
{
public:
	EventProcessor(SessionManager* pSessMgr, WorkerGroup* pWorkGroup);

	~EventProcessor();

	void regEventServer(EpollServer* pEpollSvr)
	{
		_epoll_svr_ptr = pEpollSvr;
	}

	bool addEvent(Event event);

	bool getCommand(DataXCmd* pCmd);

protected:
	virtual void doIt();

private:
	void processRead(Event& event);
	void processWrite(Event& event);
	void processClose(Event& event);

	void notifyUserDrop(int seqno);

private:
	SessionManager* _sess_mgr_ptr;
	EpollServer* _epoll_svr_ptr;
	WorkerGroup* _work_group_ptr;

	Queue<Event> _event_queue;

};

#endif //EVENT_PROCESSOR_H
