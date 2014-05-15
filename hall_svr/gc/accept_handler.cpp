
/* ======================================================
* 
* file:		accept_handler.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-11 15:47:13
* 
* ======================================================*/

#include "accept_handler.h"

AcceptHandler::AcceptHandler(SessionManager* sess_mgr_ptr)
	:session_manager_(sess_mgr_ptr)
{
}

/**
* brief:
*
* @param fd
* @param event
*/
void AcceptHandler::handle(EventInfo event_info)
{
	int fd = event_info.event_fd;

	sockaddr_in addr;
	socklen_t len;
	int client_fd = ::accept(fd, (sockaddr*)&addr, &len, 0);
	if(client_fd < 0)
	{
		if(EINTR == errno)
			continue;

		return;
	}
	//TODO set the client fd to no-block
	
	SessionBase* session = session_manager_->getIdleSession();
	if(NULL == session)
	{
		return;
	} 
	
	session->setFd(client_fd);
	session_manager_->addSession(session);	

	//add event handler
	int seqno = session->getSeqno();

	struct epoll_event event;
	event.data.u64 = client_fd << 32 | seqno;
	event.events = EPOLLIN | EPOLLET;
}

