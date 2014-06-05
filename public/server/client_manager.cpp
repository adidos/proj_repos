#include "client_manager.h"

int ClientManager::addClient(int64_t uid, int seqno)
{
	CScopeGuard guard(_mutex);
	_client_session_array[uid] = (seqno);

	return 0;
}

int ClientManager::resetClient(int64_t uid, int seqno)
{
	CScopeGuard gaurd(_mutex);
	_client_session_array[uid] = seqno;
}

int ClientManager::getSessID(int64_t uid)
{
	CScopeGuard guard(_mutex);
	Iterator iter = _client_session_array->find(uid);
	if(iter == _client_session_array.end())
	{
		LOG4CPLUS_ERROR(CLogger::logger, "Cant't find user " << uid << " session id");
		return -1;
	}

	return iter->second;
}

int ClientManager::freeClient(int64_t uid)
{
	CScopeGuard guard(_mutex);
	return _client_session_array.erase(uid);
}

int64_t ClientManager::getUid8Sid(int seqno)
{
	CScopeGuard guard(_mutex);
	Iterator iter = _client_session_array.begin();
	for( ; iter != _client_session_array.end(); ++iter)
	{
		if(seqno == iter->second)
			return iter->first;
	}
}