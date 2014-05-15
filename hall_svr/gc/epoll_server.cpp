
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
#include "hall_common.h"

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

int EpollServer::notify(int fd, int type)
{
	if(NOTIFY_DEL == type)
	{
		epoll_.
	}
	SessionBase* pSession = session_mgr_ptr_->getIdleSession();
	if(NULL == session)
	{
		return -1;
	}

	pSession->setFd(fd);
	session_mgr_ptr_->addSession(pSession);

	int seqno = pSession->getSeqno();	
	
	

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
		int ret = epoll_wait(epoll_fd_, events_, EPOLL_MAX_SIZE, -1);
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;
			break;
		}
		
		IEventHandler* pHandler = NULL;
		for(int i = 0; i < ret; ++i)
		{
			int fd = events_[i].data.u64 >> 32;

			pHandler = handler_array_[fd];
			if(NULL == pHandler)
			{
				//TODO log
			}

			if(events_[i].events & (EPOLLHUP | EPOLLERR))
			{
				this->delEvent(fd);
				handler_array_.erase(fd);
			}
			else if(events_[i].events & EPOLLIN)
			{
			}
			else if(events_[i].events & EPOLLOUT)
			{
			}
			pHandler->handle();
		}
	}

}

