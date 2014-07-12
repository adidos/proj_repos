/* ======================================================
* 
* file:		client_manager.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-10 17:35:47
* 
* ======================================================*/

#include "client_manager.h"
#include "common/logger.h"

#include <algorithm>
#include <bits/stl_multimap.h>

/**
* brief:
*
* @param uid
* @param seqno
*
* @returns   
*/
int ClientManager::addClient(int64_t uid, int seqno)
{
	CScopeGuard guard(_mutex);
	_seq2uid_array[seqno] = uid;
	
	Iterator iter = _client_session_array.find(uid);
	if(iter != _client_session_array.end() &&
		iter->second == seqno)
			return 0;
	
	_client_session_array.insert(pair<int64_t, int>(uid, seqno));

	return 0;
}

/**
* brief:
*
* @param uid
* @param seqnos
*
* @returns   
*/
int ClientManager::getSessID(int64_t uid, vector<int>& seqnos)
{
	CScopeGuard guard(_mutex);
	pair<Iterator, Iterator> ret = _client_session_array.equal_range(uid);
	if(ret.first == ret.second)
	{
		LOG4CPLUS_WARN(FLogger, "Cant't find user " 
				<< uid << " session id");
		return -1;
	}
	
	for(Iterator iter = ret.first; iter != ret.second; ++iter)
	{
		seqnos.push_back(iter->second);
	}

	return seqnos.size();
}

/**
* brief:
*
* @param seqno
*
* @returns   
*/
int ClientManager::freeClient(int seqno)
{
	CScopeGuard guard(_mutex);
	int64_t uid = _seq2uid_array[seqno];

	_seq2uid_array.erase(seqno);

	_client_session_array.erase(uid);

	return 0;
}

/**
* brief:
*
* @param uid
* @param seqno
*
* @returns   
*/
int ClientManager::freeClient(int64_t uid, int seqno)
{
	CScopeGuard guard(_mutex);
	_seq2uid_array.erase(seqno);
	
	pair<Iterator, Iterator> ret = _client_session_array.equal_range(uid);
	if(ret.first == ret.second)
	{
		return 0;
	}
	
	for(Iterator iter = ret.first; iter != ret.second; ++iter)
	{
		if(iter->second == seqno)
		{
			_client_session_array.erase(iter);
			return 0;
		}
	}

	return 0;
}

/**
* brief:
*
* @param seqno
*
* @returns   
*/
int64_t ClientManager::getUid8Sid(int seqno)
{
	CScopeGuard guard(_mutex);
	std::map<int, int64_t>::iterator iter = _seq2uid_array.find(seqno);
	if(iter == _seq2uid_array.end())
	{
		LOG4CPLUS_WARN(FLogger, "can't find userid by seqno[" << seqno <<"].");
		return -1;
	}
	return iter->second;
}
