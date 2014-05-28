
/* ======================================================
* 
* file:		session_manager.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-13 11:02:28
* 
* ======================================================*/

#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <stdint.h>

#include <map>
#include <list>

using namespace std;

#include "session.h"
#include "thread_sync.h"

class SessionManager
{
public:
	SessionManager();
	~SessionManager();

	SessionBase* getIdleSession();
	void freeSession(SessionBase* pSession);

	SessionBase* getSession(int seqno);
	bool addSession(SessionBase* pSession);
	void delSession(int seqno);
	
	typedef map<int, SessionBase*>::iterator Iterator;
private:
	int cur_seq_;

	list<SessionBase*> idle_array_; //session使用完后进行回收再利用
	CMutex idle_mutex_;
	
	map<int, SessionBase*> session_array_;	//当前所有的有效session
	CMutex session_mutex_;
};

#endif /*SESSION_MANAGER_H*/
