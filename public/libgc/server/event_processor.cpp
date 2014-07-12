
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
#include "client_manager.h"
#include "worker_group.h"
#include "session_manager.h"
#include "epoll_server.h"

#include "common/configure.h"
#include "common/index.h"
#include "common/logger.h"
#include "common/utility.h"

#include <algorithm>

extern Configure* g_pConfig;

EventProcessor::EventProcessor(SessionManager* pSessMgr,
	WorkerGroup* pWorkGroup): _sess_mgr_ptr(pSessMgr),
	_epoll_svr_ptr(NULL),_work_group_ptr(pWorkGroup),
	_event_queue(QUEUE_SIZE)
{
	_client_mgr_ptr = new ClientManager();
}

EventProcessor::~EventProcessor()
{
	if(NULL != _client_mgr_ptr)
	{
		delete _client_mgr_ptr;
		_client_mgr_ptr = NULL;
	}
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
*/
void EventProcessor::doIt()
{
	LOG4CPLUS_DEBUG(FLogger, "event processor start work!");
	while(!_terminate)
	{
		Event event;
		bool bSucc = _event_queue.pop(event, 60*1000);
		if(!bSucc)
		{
			LOG4CPLUS_DEBUG(FLogger, "event queue is empty!");
			continue;
		}

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
	LOG4CPLUS_DEBUG(FLogger, "Process read Event " << event.dump());

	int seqno = event.seqno;

	SessionBasePtr pSession = _sess_mgr_ptr->getSession(seqno);
	if(!pSession)
	{
		LOG4CPLUS_WARN(FLogger, "session is null, seqno = "<<seqno);

		_sess_mgr_ptr->delSession(seqno);

		_client_mgr_ptr->freeClient(seqno);

		return ;
	}
	
	int ret = pSession->recvBuffer();
	if(SOCKET_CLOSE == ret || SOCKET_ERR == ret) 
	{
		event.type = EVENT_CLOSE;
		processClose(event);
	}
	else if(ret > 0)
	{
		while(1) //may be receive buffer contain not only one commands
		{
			DataXCmdPtr pCmd;
			ret = pSession->parseProtocol(pCmd);
			if(ret != 0 || ! pCmd) break;

			int64_t uid = pCmd->get_userid();
			handleClient(uid, seqno);

			CmdTask task;
			task.idx = Index::get();
			task.pCmd = pCmd;
			task.seqno = seqno;
			task.timestamp = current_time_usec();

			bool bSucc = _work_group_ptr->dispatch(task);
			if(!bSucc)
			{
				LOG4CPLUS_ERROR(FLogger, "insert command to receive"
						<< " queue failed.");
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
	LOG4CPLUS_DEBUG(FLogger, "Process write Event " << event.dump());

	int seqno = event.seqno;
	SessionBasePtr pSession = _sess_mgr_ptr->getSession(seqno);
	if(!pSession)
	{
		LOG4CPLUS_WARN(FLogger, "session is null, seqno = "<<seqno);

		_sess_mgr_ptr->delSession(seqno);

		_client_mgr_ptr->freeClient(seqno);

		return ;
	}

	int fd = pSession->getFd();
	int64_t data = U64(seqno, fd);

	_epoll_svr_ptr->notify(fd, data, EVENT_READ);

	pSession->sendBuffer();
}

/**
* brief: process close event
*
* @param event
*/
void EventProcessor::processClose(Event& event)
{
	LOG4CPLUS_DEBUG(FLogger, "Process close Event " << event.dump());

	int seqno = event.seqno;

	int64_t uid = _client_mgr_ptr->getUid8Sid(seqno);
	if(uid > 0)
	{
		notifyUserDrop(uid);
		_client_mgr_ptr->freeClient(uid, seqno);

	}
		
	SessionBasePtr pSession = _sess_mgr_ptr->getSession(seqno);
	if(pSession)
	{
		int fd = pSession->getFd();
		_sess_mgr_ptr->delFd(fd);

		pSession->close();
	}
	
	_sess_mgr_ptr->delSession(seqno);

}

void EventProcessor::handleClient(uint64_t uid, int seqno)
{
	vector<int> seqnos;
	int ret = _client_mgr_ptr->getSessID(uid, seqnos);
	if(-1 == ret) //new client
	{
		_client_mgr_ptr->addClient(uid, seqno);
	}
	else
	{
		vector<int>::iterator iter = find(seqnos.begin(), seqnos.end(),seqno);
		if(iter == seqnos.end()) //新的终端连接
		{
			_client_mgr_ptr->addClient(uid, seqno);
			iter = seqnos.begin();
			for(; iter != seqnos.end(); ++iter)
			{
				notifyUserRelogin(uid, *iter);//TODO是否需要通知重复登陆的数量
			}
		}
		//else  已经记录过该连接
	}
}

void EventProcessor::notifyUserDrop(int64_t uid)
{	
	DataXCmdPtr pCmd(new DataXCmd("UserDrop"));
	pCmd->set_userid(uid);

	CmdTask task;
	task.idx = Index::get();
	task.pCmd = pCmd;
	task.timestamp = current_time_usec();

	_work_group_ptr->dispatch(task);
}

void EventProcessor::notifyUserRelogin(int64_t uid, int seqno)
{
	DataXCmdPtr pCmd(new DataXCmd("UserRelogin"));
	pCmd->set_userid(uid);

	CmdTask task;
	task.idx = Index::get();
	task.pCmd = pCmd;
	task.seqno = seqno;
	task.timestamp = current_time_usec();

	_work_group_ptr->dispatch(task);
}

