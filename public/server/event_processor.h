
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
	EventProcessor(SessionManager* pSessMgr);

	~EventProcessor();

	void regEventServer(EpollServer* pEpollSvr)
	{
		_epoll_svr_ptr = pEpollSvr;
	}

	bool addEvent(CEvent event);

	bool getCommand(DataXCmd* pCmd);

protected:
	virtual void doIt();

private:
	void processRead(CEvent& event);
	void processWrite(CEvent& event);
	void processClose(CEvent& event);

	void userDrop(int seqno);

private:
	SessionManager* _sess_mgr_ptr;
	EpollServer* _epoll_svr_ptr;

	Queue<CEvent> _event_queue;
	Queue<DataXCmd*> _cmd_queue;

};

#endif //EVENT_PROCESSOR_H
