
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

#include "common/queue.h"
#include "common/thread.h"
#include "common/DataXCmd.h"

class ClientManager;
class WorkerGroup;
class EpollServer;
class SessionManager;


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

	ClientManager* getClientMgr()
	{
		return _client_mgr_ptr;
	}

protected:
	virtual void doIt();

private:
	void processRead(Event& event);
	void processWrite(Event& event);
	void processClose(Event& event);

	void handleClient(uint64_t uid, int seqno);

	void notifyUserDrop(int64_t uid);
	void notifyUserRelogin(int64_t uid, int seqno);

private:
	SessionManager* _sess_mgr_ptr;
	EpollServer* _epoll_svr_ptr;
	WorkerGroup* _work_group_ptr;
	ClientManager* _client_mgr_ptr;

	Queue<Event> _event_queue;
};

#endif //EVENT_PROCESSOR_H
