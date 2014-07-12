
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

/**
* brief:添加一个新的fd-seqno，如果fd已经存在，fd被重用
*       更新为新的seqno，返回老的seqno
* @param fd
* @param seqno
*
* @returns   
*/
int SessionManager::addFd(int fd, int seqno)
{
	CScopeGuard guard(_fd_mutex);

	int old_seq = 0;

	map<int, int>::iterator iter = _fd_array.find(fd);
	if(iter != _fd_array.end())
	{
		LOG4CPLUS_WARN(FLogger, "fd " << fd  << " was used by session["
			<< iter->second  << "] .");

		old_seq = iter->second;
		
	}

	_fd_array[fd] = seqno;

	LOG4CPLUS_TRACE(FLogger, "add a fd-seqno [" << fd << ", "
			<< seqno << "].");

	return old_seq;
}

int SessionManager::getSeqno8Fd(int fd)
{
	CScopeGuard guard(_fd_mutex);
	map<int, int>::iterator iter = _fd_array.find(fd);
	if(iter != _fd_array.end())
	{
		LOG4CPLUS_TRACE(FLogger, "get seqno " << iter->second
			<< " success by fd " << fd);

		return iter->second;
	}

	LOG4CPLUS_WARN(FLogger, "get seqno failed" );

	return -1;
}

int SessionManager::delFd(int fd)
{
	CScopeGuard guard(_fd_mutex);
	
	_fd_array.erase(fd);

	LOG4CPLUS_TRACE(FLogger, "erase fd " << fd);

	return 0;
}

/**
* brief: 
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
	
	LOG4CPLUS_TRACE(FLogger, "erase a session by " << pSession->getSeqno()
			<< " from manager!");
}

void SessionManager::delSession(int seqno)
{
	CScopeGuard guard(session_mutex_);
	session_array_.erase(seqno);
	
	LOG4CPLUS_TRACE(FLogger, "erase a session by " << seqno  << " from manager!");
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

	LOG4CPLUS_TRACE(FLogger, "get a seesion[" << iter->second->getSeqno()
		<< "] success by " << seqno);

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

	LOG4CPLUS_TRACE(FLogger, "add a session[" << seqno <<"] into manager, result"
			<< " is " << result.second);
		
	return result.second;	
}

