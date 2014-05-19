
/* ======================================================
* 
* file:		epoll_server.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-04 19:53:03
* 
* ======================================================*/

#include "epoll_server.h"
#include "gc_logger.h"
#include "session_manager.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <utility>

using namespace std;

EpollServer::EpollServer(SessionManager* pSessionMgr)
 :session_mgr_ptr_(pSessionMgr)
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
	epoll_.create(size);

	return 0;
}

int EpollServer::notify(int fd)
{
	SessionBase* pSession = session_mgr_ptr_->getIdleSession();
	if(NULL == pSession)
	{
		return -1;
	}

	pSession->setFd(fd);
	session_mgr_ptr_->addSession(pSession);

	int seqno = pSession->getSeqno();	
	int64_t data = (uint64_t)fd << 32 | seqno;
	
	epoll_.add(fd, data, EPOLLIN);	
	LOG4CPLUS_DEBUG(GCLogger::ROOT, "add a Session, fd = " << fd
			<< ", seqno = " << seqno);

	return 0;
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
		int ret = epoll_.wait(-1);
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;

			LOG4CPLUS_ERROR(GCLogger::ROOT, "epoll server error, exit run loop...");
			break;
		}
		
		for(int i = 0; i < ret; ++i)
		{
			struct epoll_event ev = epoll_->get(i);

			if(ev.events & (EPOLLHUP | EPOLLERR))
			{
				LOG4CPLUS_TRACE(GCLogger::ROOT, "epoll error, fd = " << FD(ev.data.u64)
					<< ", seqno = " << SEQ(ev.data.u64));

				handlerError(ev.data.u64);
			}
			else if(ev.events & EPOLLIN)
			{
				LOG4CPLUS_TRACE(GCLogger::ROOT, "epoll readable, fd = " << FD(ev.data.u64)
					<< ", seqno = " << SEQ(ev.data.u64));
				handlerRead(ev.data.u64);
			}
			else if(ev.events & EPOLLOUT)
			{
				LOG4CPLUS_TRACE(GCLogger::ROOT, "epoll writeable, fd = " << FD(ev.data.u64)
					<< ", seqno = " << SEQ(ev.data.u64));
				handlerWrite(ev.data.u64);
			}
		}
	}

}

/**
* brief:
*
* @param event
*
* @returns   
*/
int EpollServer::handlerError(uint64_t data)
{	
	int seqno = SEQ(data);
	int fd = FD(data);

	epoll_.del(fd, data, 0);

}

/**
* brief:
*
* @returns   
*/
int EpollServer::handlerRead(uint64_t data)
{
	int seqno = SEQ(data);
	int fd = FD(data);
	SessionBase* pSession = session_mgr_ptr_->getSession(seqno);
	assert(NULL != pSession)
	
	int ret = pSession->recv();
	if(SOCKET_CLOSE == ret ||
		SOCKET_ERR == ret) //client close the socket
	{
		//handle in up layer， if handle earlier , the session will recycle,
		//the up layer will can't find the session
		//pSession->setFd(-1);
		//session_mgr_ptr_->delSession(seqno);
		//session_mgr_ptr_->pushBack(pSession);
		
		epoll_.del(fd, data, 0);

		//TODO event notify
	}
	else if(ret > 0)
	{
		
	}
	
	//TODO event info notify
}

/**
* brief:
*
* @returns   
*/
int EpollServer::handlerWrite(uint64_t data)
{	
	int seqno = SEQ(data);
	int fd = FD(data);

	//modify event to EPOLLIN
	epoll_.mod(fd, data, EPOLLIN);

	//notify send thread write event
	
}

