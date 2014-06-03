
/* ======================================================
* 
* file:		_epollserver.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-04 19:53:03
* 
* ======================================================*/

#include "_epollserver.h"
#include "logger.h"
#include "session_manager.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <utility>

using namespace std;

EpollServer::EpollServer()
{
}

/**
* brief:
*
* @param size
*
* @returns   
*/
int EpollServer::init(int size)
{
	_epoll.create(size);

	return 0;
}

int EpollServer::notify(int fd, uint64_t data, int iEvent)
{
	switch(iEvent)
	{
		case EVENT_NEW:
			_epoll.add(fd, data, EPOLLET | EPOLLIN);	
			break;

		case EVENT_READ:
			_epoll.mod(fd, data, EPOLLET | EPOLLIN);
			break;

		case EVENT_WRITE:
			_epoll.mod(fd, data, EPOLLET | EPOLLOUT);
			break;

		case EVENT_CLOSE:
		case EVENT_ERROR:
			_epoll.del(fd, data, EPOLLET | EPOLLIN | EPOLLOUT);
			break;

		default:
			LOG4CPLUS_ERROR(CLogger::logger, "notify unknow event, " << iEvent);
	}
}

/**
* brief:
*
* @returns   
*/
void EpollServer::doIt()
{
	while(!teminate_)
	{
		//block until event occure
		int ret = _epoll.wait(-1);
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;

			LOG4CPLUS_ERROR(CLogger::logger, "epoll server error, exit run loop...");
			break;
		}
		
		LOG4CPLUS_TRACE(CLogger::logger, "epoll wait return value " << ret);
		for(int i = 0; i < ret; ++i)
		{
			struct _epollevent ev = _epoll->get(i);

			CEvent event;
			event.id = H32(ev.data.u64)
			event.timestamp = Utilities::
			if(ev.events & (EPOLLHUP | EPOLLERR))
			{
				LOG4CPLUS_TRACE(CLogger::logger, "epoll error, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));
				
				processError(ev.data.u64);
			}
			else if(ev.events & EPOLLIN)
			{
				LOG4CPLUS_TRACE(CLogger::logger, "epoll readable, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));

				processRead(ev.data.u64);
			}
			else if(ev.events & EPOLLOUT)
			{
				LOG4CPLUS_TRACE(CLogger::logger, "epoll writeable, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));

				processWrite(ev.data.u64);
			}
		}
	}

}

/**
* brief:
*
* @returns   
*/
int EpollServer::processRead(uint64_t data)
{	
	int seqno = H32(data);


	SessionBase* pSession = _session_mgr_ptr->getSession(seqno);
	if(NULL == pSession)
	{
		LOG4CPLUS_WARN(CLogger::logger, "session is null, seqno = " << seqno);
		return -1;
	}
	
	int ret = pSession->recv();
	if(SOCKET_CLOSE == ret || SOCKET_ERR == ret) 
	{
		processError(data);
	}
	else if(ret > 0)
	{
		
		while(1)
		{
			DataXCmd* pCmd = NULL;
			ret = pCmd->parseProtocol(pCmd);
			if(ret != 0) break;
		
			bool bSucc = _servant_ptr->insertRecvQueue(pCmd);
			if(!bSucc)
			{
				LOG4CPLUS_ERROR(CLogger::logger, "insert command to receive queue failed.");
				return -1;
			}
		}
	}

	return 0;
}

/**
* brief:
*
* @returns   
*/
int EpollServer::processWrite(uint64_t data)
{	
	return 0;
}

/**
* brief:
*
* @param data
*
* @returns   
*/
int EpollServer::processError(uint64_t data)
{
	int fd = L32(data);
	int seqno = H32(data);

	_epoll.del(fd, data, EPOLLOUT|EPOLLIN|EPOLLET);

	SessionBase* pSession = _session_mgr_ptr->getSession(seqno);
	if(NULL != pSession)
	{
		_session_mgr_ptr->freeSession(pSession);
	}

	//TODO NotifyUserDrop

	return 0;	
}

