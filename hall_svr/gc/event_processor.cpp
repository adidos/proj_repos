
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

EventProcessor::EventProcessor(SessionManager* pSessMgr, 
	EpollServer* pEpollSvr) : _sess_mgr_ptr(pSessMgr), 
	_epoll_svr_ptr(pSessMgr), _event_queue(QUEUE_SIZE),
	_event_queue(QUEUE_SIZE)
{

}

~EventProcessor()
{
}

/**
* brief:
*
* @param event
*
* @returns   
*/
bool EventProcessor::addEvent(CEvent event)
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
	return _cmd_queue.pop(pCmd, QUEUE_WAIT_MS);
}

/**
* brief: 
*/
void EventProcessor::doIt()
{
	while(true)
	{
		CEvent event;
		bool bSucc = _event_queue.pop(evente, 100*QUEUE_WAIT_MS);
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
void EventProcessor::processRead(CEvent& event)
{
	int seqno = event.uid;

	SessionBase* pSession = _session_mgr_ptr->getSession(seqno);
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
		
			bool bSucc = _cmd_queue->push(pCmd);
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
void EventProcessor::processWrite(CEvent& event)
{
	assert(event.uid > 0);

	int seqno = event.uid;
	SessionBase* pSession = _session_mgr_ptr->getSession(seqno);
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
void EventProcessor::processClose(CEvent& event)
{
	assert(event.uid > 0);

	int seqno = event.uid;
	SessionBase* pSession = _session_mgr_ptr->getSession(seqno);
	if(NULL == pSession)
	{
		LOG4CPLUS_WARN(CLogger::logger, "session is null, seqno = "<<seqno);
		return ;
	}

	int fd = pSession->getFd();
	int64_t data = U64(seqno, fd);
	_epoll_svr_ptr->notify(fd, data, EVENT_CLOSE);

	_session_mgr_ptr->freeSession(pSession);

	//TODO NotifyUserDrop
	//TODO remove the relation between userid and seqno
}

