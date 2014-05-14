
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
void AcceptHandler::handle(int fd, int event)
{
	sockaddr_in addr;
	socklen_t len;
	int client_fd = ::accept(fd, (sockaddr*)&addr, &len, 0);
	//TODO set the client fd to no-block
	
	//whether the fd is reuse
	SessionBase* session = session_manager_->getSession8Fd(client_fd);
	if(NULL != session)
	{
		//reuse ,notify application layer, close old session
		//get ident by fd or sessid, offline
	} 
	else
	{
		session = session_manager_->getIdleSession();
	}
	assert(NULL != session);

	SessID sid ;
	sid.fd = client_fd;
	sid.seqno = SessionManager::newSeqNo();

	session->setSessId(sid);
	session_manager_->addSession(client_fd, session);	
}

/**
* brief: if this function called, whether means the process can exit
*
* @param fd
*/
void AcceptHandler::handleError(int fd)
{
	

}
