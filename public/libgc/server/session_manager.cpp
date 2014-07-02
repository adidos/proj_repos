
/* ======================================================
* 
* file:		session_manager.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-13 15:50:07
* 
* ======================================================*/

#include "session_manager.h"
#include "common/logger.h"
#include "common/index.h"

SessionManager::SessionManager()
{
	session_array_.clear();
}

SessionManager::~SessionManager()
{
	session_array_.clear();
}

int SessionManager::addFd(int fd, int seqno)
{
	CScopeGuard guard(_fd_mutex);

	_fd_array[fd] = seqno;

	return 0;
}

int SessionManager::getSeqno8Fd(int fd)
{
	CScopeGuard guard(_fd_mutex);
	map<int, int>::iterator iter = _fd_array.find(fd);
	if(iter != _fd_array.end())
	{
		return iter->second;
	}

	return -1;
}

/**
* brief: after using, put session back to idle session array
*
* @param pSession
*/
void SessionManager::delSession(SessionBasePtr pSession)
{
	if(!pSession)
	{
		LOG4CPLUS_ERROR(FLogger, "INVALID paramer...");
		return;
	}

	CScopeGuard guard(session_mutex_);
	session_array_.erase(pSession->getSeqno());
}

void SessionManager::delSession(int seqno)
{
	CScopeGuard guard(session_mutex_);
	session_array_.erase(seqno);
}
/**
* brief:
*
* @param seqno
*
* @returns   
*/
SessionBasePtr SessionManager::getSession(int seqno)
{
	CScopeGuard guard(session_mutex_);	
	Iterator iter = session_array_.find(seqno);
	if(iter == session_array_.end())
	{
		LOG4CPLUS_ERROR(FLogger, "session[" << seqno << "] is not exist!");
		return SessionBasePtr();
	}

	return iter->second;
}

/**
* brief:
*
* @param pSession
*
* @returns   
*/
bool SessionManager::addSession(SessionBasePtr pSession)
{
	if(!pSession)
	{
		LOG4CPLUS_ERROR(FLogger, "INVALID paramer...");
		return false;
	}
	
	int seqno = pSession->getSeqno();

	CScopeGuard guard(session_mutex_);	
	pair<Iterator, bool> result = session_array_.insert(make_pair(seqno, pSession));
		
	return result.second;	
}

