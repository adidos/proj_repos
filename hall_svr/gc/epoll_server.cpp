
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
	int64_t data = U64(seqno, fd);
	
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
		
		LOG4CPLUS_TRACE(GCLogger::ROOT, "epoll wait return value " << ret);
		for(int i = 0; i < ret; ++i)
		{
			struct epoll_event ev = epoll_->get(i);

			NotifyInfo notify;
			if(ev.events & (EPOLLHUP | EPOLLERR))
			{
				LOG4CPLUS_TRACE(GCLogger::ROOT, "epoll error, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));
				
				epoll_.del(fd, ev.data.u64, 0);

				notify.type = NT_CLOSE;
				notify.seqno = H32(ev.data.u64);
				notify_handler_ptr_->handle(notify);	
			}
			else if(ev.events & EPOLLIN)
			{
				LOG4CPLUS_TRACE(GCLogger::ROOT, "epoll readable, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));

				handlerRead(ev.data.u64);
			}
			else if(ev.events & EPOLLOUT)
			{
				LOG4CPLUS_TRACE(GCLogger::ROOT, "epoll writeable, fd = " << ev.data.fd
					<< ", seqno = " << H32(ev.data.u64));

				handlerWrite(ev.data.u64);
			}
		}
	}

}

/**
* brief:
*
* @returns   
*/
int EpollServer::handlerRead(uint64_t data)
{	
	int seqno = H32(data);

	NotifyInfo notify;
	notify.seqno = seqno;

	SessionBase* pSession = session_mgr_ptr_->getSession(seqno);
	if(NULL == pSession)
	{
		LOG4CPLUS_WARN(GCLogger::ROOT, "session is null, seqno = " << seqno);
		return -1;
	}
	
	int ret = pSession->recv();
	if(SOCKET_CLOSE == ret || SOCKET_ERR == ret) 
	{
		epoll_.del(L32(data), data, 0);
		notify.type = NT_CLOSE;
	}
	else if(ret > 0)
	{
		notify.type = NT_READ;	
	}

	notify_handler_ptr_->handle(notify);	
	return 0;
}

/**
* brief:
*
* @returns   
*/
int EpollServer::handlerWrite(uint64_t data)
{	
	epoll_.mod(L32(data), data, EPOLLIN);

	NotifyInfo notify;
	notify.seqno = U32(data);
	notify.type = NT_WRITE;	

	notify_handler_ptr_->handle(notify);
	return 0;
}

