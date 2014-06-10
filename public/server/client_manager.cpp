#include "client_manager.h"
#include "common/logger.h"

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

int ClientManager::getSessID(int64_t uid, vector<int>& seqnos)
{
	CScopeGuard guard(_mutex);
	pair<Iterator, Iterator> ret = _client_session_array.eqaul_range(uid);
	if(ret.first == ret.second)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "Cant't find user " 
				<< uid << " session id");
		return -1;
	}
	
	for(Iterator iter = ret.first; iter != ret.second; ++iter)
	{
		seqnos.push_back(iter->second);
	}

	return seqnos.size();
}

int ClientManager::freeClient(int64_t uid, int seqno)
{
	CScopeGuard guard(_mutex);
	_seq2uid_array.erase(seqno);
	
	pair<Iterator, Iterator> ret = _client_session_array.eqaul_range(uid);
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

int64_t ClientManager::getUid8Sid(int seqno)
{
	CScopeGuard guard(_mutex);
	std::map<int, int64_t>::iterator iter = _seq2uid_array.find(seqno);
	if(iter == _seq2uid_array.end())
	{
		LOG4CPLUS_WARN(CLogger::logger, "can't find user by seqno[" << seqno <<"].")
		return -1;
	}
	return iter->second;
}
