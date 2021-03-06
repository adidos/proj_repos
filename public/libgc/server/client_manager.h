
/* ======================================================
* 
* file:		client_manager.h
* brief:	记录用户标识与session的对应关系
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-29 10:55:31
* 
* ======================================================*/
#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <stdint.h>

#include <map>
#include <vector>

#include "common/thread_sync.h"

using namespace std;

class ClientManager
{
public:
	ClientManager(){};
	~ClientManager()
	{
		_client_session_array.clear();
	}

	int addClient(int64_t uid, int seqno);

	int getSessID(int64_t uid, vector<int>& seqnos);
	int64_t getUid8Sid(int seqno);

	int freeClient(int64_t uid, int seqno);
	int freeClient(int seqno);

private:
	typedef multimap<int64_t, int>::iterator Iterator;

	multimap<int64_t, int> _client_session_array;

	map<int, int64_t> _seq2uid_array;

	CMutex _mutex;
};

#endif //CLIENT_MANAGER_H
