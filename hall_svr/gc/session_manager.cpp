
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

SessionManager::SessionManager()
	:cur_seq_(1)
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
		ptr = idle_array_.front();
		idle_array_.pop_front();
	}
	else
	{
		ptr = new SessionBase();
		ptr->setSeqno(cur_seq_++);
	}
	
	return ptr;
}

/**
* brief: after using, put session back
*
* @param pSession
*/
void SessionManager::pushBack(SessionBase* pSession)
{
	if(NULL == pSession)
	{
		return;
	}
	//pSession->reset();
	
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
	Iterator iter = session_array_.find(seqno);
	if(iter == session_array_.end())
		return NULL;

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
		return false;
	
	int seqno = pSession->getSeqno();
	pair<Iterator, bool> result = session_array_.insert(make_pair(seqno, pSession));
	
	return result.second;	
}
