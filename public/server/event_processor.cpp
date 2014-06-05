
/* ======================================================
* 
* file:		event_processor.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-29 14:41:26
* 
* ======================================================*/

#include "event_processor.h"
#include "configure.h"
#include "index.h"

extern Configure* g_pConfig;

EventProcessor::EventProcessor(SessionManager* pSessMgr£¬
	WorkerGroup* pWorkGroup): _sess_mgr_ptr(pSessMgr),
	_epoll_svr_ptr(NULL),_work_group_ptr(pWorkGroup)
	_event_queue(QUEUE_SIZE), _event_queue(QUEUE_SIZE)
{

}

EventProcessor::~EventProcessor()
{
}

/**
* brief:
*
* @param event
*
* @returns   
*/
bool EventProcessor::addEvent(Event event)
{
	return _event_queue.push(event, QUEUE_WAIT_MS);	
}

/**
* brief:
*
* @param pCmd
*
* @returns   
*/
bool EventProcessor::getCommand(DataXCmd* pCmd)
{
	return _cmd_queue.pop(pCmd, QUEUE_WAIT_MS * 100);
}

/**
* brief: 
*/
void EventProcessor::doIt()
{
	while(true)
	{
		Event event;
		bool bSucc = _event_queue.pop(event, 60*1000);
		if(!bSucc)
		{
			LOG4CPLUS_DEBUG(CLogger::logger, "event queue is empty!");
			continue;
		}

		LOG4CPLUS_DEBUG(CLogger::logger, "Process Event" << event.dump());
		if(EVENT_CLOSE == event.type)
		{
			processClose(event);
		}
		else if(EVENT_READ == event.type)
		{
			processRead(event);
		}
		else if(EVENT_WRITE == event.type)
		{
			processWrite(event);
		}
	}

}

/**
* brief: process read event
*
* @param event
*/
void EventProcessor::processRead(Event& event)
{
	int seqno = event.uid;

	SessionBase* pSession = _sess_mgr_ptr->getSession(seqno);
	if(NULL == pSession)
	{
		LOG4CPLUS_WARN(CLogger::logger, "session is null, seqno = "<<seqno);
		return ;
	}
	
	int ret = pSession->recv();
	if(SOCKET_CLOSE == ret || SOCKET_ERR == ret) 
	{
		event.type = EVENT_CLOSE;
		processClose(event);
	}
	else if(ret > 0)
	{
		while(1) //may be receive buffer contain two or more commands
		{
			DataXCmd* pCmd = NULL;
			ret = pSession->parseProtocol(pCmd);
			if(ret != 0) break;

			CmdTask task;
			task.idx = Index::get();
			task.pCmd = pCmd;
			task.timestamp = current_time_usec();

			LOG4CPLUS_DEBUG(CDebugLogger::logger, "TimeTrace: event->task spend time " 
				<< task.timestamp - event.timestamp);
			
			bool bSucc = _work_group_ptr->dispatch(task, QUEUE_WAIT_MS);
			if(!bSucc)
			{
				LOG4CPLUS_ERROR(CLogger::logger, "insert command to receive"
						<< " queue failed.");
				return ;
			}
		}
	}
}

/**
* brief: process write event
*
* @param event
*/
void EventProcessor::processWrite(Event& event)
{
	assert(event.uid > 0);

	int seqno = event.uid;
	SessionBase* pSession = _sess_mgr_ptr->getSession(seqno);
	if(NULL == pSession)
	{
		LOG4CPLUS_WARN(CLogger::logger, "session is null, seqno = "<<seqno);
		return ;
	}

	int fd = pSession->getFd();
	int64_t data = U64(seqno, fd);
	_epoll_svr_ptr->notify(fd, data, EVENT_READ);
	pSession->send();
}

/**
* brief: process close event
*
* @param event
*/
void EventProcessor::processClose(Event& event)
{
	assert(event.uid > 0);

	int seqno = event.uid;
	SessionBase* pSession = _sess_mgr_ptr->getSession(seqno);
	if(NULL == pSession)
	{
		LOG4CPLUS_WARN(CLogger::logger, "session is null, seqno = "<<seqno);
		return ;
	}

	int fd = pSession->getFd();
	int64_t data = U64(seqno, fd);
	_epoll_svr_ptr->notify(fd, data, EVENT_CLOSE);

	_sess_mgr_ptr->freeSession(pSession);

	notifyUserDrop(seqno);
}

void EventProcessor::notifyUserDrop(int seqno)
{
	DataXCmd* pCmd = new DataXCmd("UserDrop");
	pCmd->set_userid(seqno);

	CmdTask task;
	task.idx = Index::get();
	task.pCmd = pCmd;
	task.timestamp = current_time_ms();

	_work_group_ptr->dispatch(task, QUEUE_WAIT_MS);
}

