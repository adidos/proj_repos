
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
#include "common/thread_sync.h"

class SessionManager
{
public:
	SessionManager();
	~SessionManager();

	SessionBasePtr getSession(int seqno);
	bool addSession(SessionBasePtr pSession);
	void delSession(SessionBasePtr pSession);
	void delSession(int seqno);

	int addFd(int fd, int seqno);
	int getSeqno8Fd(int fd);
	
	typedef map<int, SessionBasePtr>::iterator Iterator;
private:
	map<int, SessionBasePtr> session_array_;	//当前所有的有效session
	CMutex session_mutex_;

	map<int, int> _fd_array;
	CMutex _fd_mutex;
};

#endif /*SESSION_MANAGER_H*/
