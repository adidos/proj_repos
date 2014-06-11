
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
	idle_array_.clear();
	session_array_.clear();
}

SessionManager::~SessionManager()
{
	list<SessionBase*>::iterator iter = idle_array_.begin();
	for(; iter != idle_array_.end(); ++iter)
	{
		delete *iter;
		*iter = NULL;
	}
	idle_array_.clear();

	map<int, SessionBase*>::iterator iter1 = session_array_.begin();
	for(; iter1 != session_array_.end(); ++iter1)
	{
		SessionBase* ptr = iter1->second;
		delete ptr;
		ptr = NULL;
	}
	session_array_.clear();
}

int SessionManager::addFd(int fd, int seqno)
{
	typedef map<int,int>::iterator fd2seqIter;
	pair<fd2seqIter, bool> ret = _fd_array.insert(make_pair(fd, seqno));

	return ret.second ? 0 : -1;
}

int SessionManager::getSeqno8Fd(int fd)
{
	CScopeGuard gaurd(_fd_mutex);
	map<int, int>::iterator iter = _fd_array.find(fd);
	if(iter != _fd_array.end())
	{
		return iter->second;
	}

	return -1;
}

/**
* brief: get a idle session 
*
* @returns  no idle session return NULL, else a pointer to session 
*/
SessionBase* SessionManager::getIdleSession()
{
	SessionBase* ptr = NULL;

	if(idle_array_.size() > 0)
	{
		CScopeGuard guard(idle_mutex_);
		ptr = idle_array_.front();
		idle_array_.pop_front();
		LOG4CPLUS_TRACE(CLogger::logger, "get a session from idle array, seqno = " 
			<< ptr->getSeqno());
	}
	else
	{
		ptr = new SessionBase();
		ptr->setSeqno(Index::get());
		LOG4CPLUS_TRACE(CLogger::logger, "idle array is empty, create a new session, "
				<< "seqno = " << ptr->getSeqno());
	}
	
	return ptr;
}

/**
* brief: after using, put session back to idle session array
*
* @param pSession
*/
void SessionManager::freeSession(SessionBase* pSession)
{
	if(NULL == pSession)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "INVALID paramer...");
		return;
	}

	{
		CScopeGuard guard(session_mutex_);
		session_array_.erase(pSession->getSeqno());
	}

	pSession->close();

	CScopeGuard guard(idle_mutex_);	
	idle_array_.push_back(pSession);
}

/**
* brief:
*
* @param seqno
*
* @returns   
*/
SessionBase* SessionManager::getSession(int seqno)
{
	CScopeGuard guard(session_mutex_);	
	Iterator iter = session_array_.find(seqno);
	if(iter == session_array_.end())
	{
		LOG4CPLUS_ERROR(CLogger::logger, "session[" << seqno << "] is not exist!");
		return NULL;
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
bool SessionManager::addSession(SessionBase* pSession)
{
	if(NULL == pSession)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "INVALID paramer...");
		return false;
	}
	
	int seqno = pSession->getSeqno();

	CScopeGuard guard(session_mutex_);	
	pair<Iterator, bool> result = session_array_.insert(make_pair(seqno, pSession));
	
	return result.second;	
}

