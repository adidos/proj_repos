
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

class SessionManager
{
public:
	SessionManager();
	~SessionManager();

	SessionBase* getIdleSession();
	void pushBack(SessionBase* pSession);

	SessionBase* getSession8Fd(int fd);
	bool addSession(int, SessionBase* pSession);
	
public:
	static int newSeqNo()
	{
		return cur_seq_++;
	}
	
private:
	list<SessionBase*> idle_array_; //session使用完后进行回收再利用
	
	map<int, SessionBase*> session_array_;	//当前所有的有效session

	static int cur_seq_;
};

#endif /*SESSION_MANAGER_H*/
