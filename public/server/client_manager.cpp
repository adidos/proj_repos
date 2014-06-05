#include "client_manager.h"

int ClientManager::addClient(int64_t uid, int seqno)
{
	CScopeGuard guard(_mutex);
	pair<Iterator, bool> ret = _client_session_array.insert(make_pair(uid, seqno));

	return ret->second ? 0 : -1;
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

int ClientManager::resetClient(int64_t uid, int seqno)
{
	CScopeGuard guard(_mutex);
	_client_session_array[uid] = seqno;
	return 0;
}

int ClientManager::freeClient(int64_t uid)
{
	CScopeGuard guard(_mutex);
	return _client_session_array.erase(uid);
}