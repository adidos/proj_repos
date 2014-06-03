
/* ======================================================
* 
* file:		session.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-11 21:28:19
* 
* ======================================================*/

#ifndef SESSION_BASE_H
#define SESSION_BASE_H

#include <string>
#include "thread_sync.h"

#define SOCKET_CLOSE	-10
#define SOCKET_ERR	-11

using namespace std;

class SessionBase
{
public:
	SessionBase();
	SessionBase(int fd, int seqno);
	~SessionBase();

	int getFd(){return fd_;}

	int getSeqno(){return seq_no_;}

	void setFd(int fd)
	{
		fd_ = fd;
	}

	void setSeqno(int seqno)
	{
		seq_no_ = seqno;
	}

	void clearBuffer();

	int recv();
	int send();
	int close();

	int write2Send(const string& buffer_send);	

	int parseProtocol(DataXCmd* pCmd);

private:
	void cmdRelease(DataXCmd* pCmd)
	{
		if(NULL != pCmd)
		{
			delete pCmd;
			pCmd = NULL;
		}
	}
	
protected:
	int fd_;
	int seq_no_;

	string send_buff_;
	string recv_buff_;

	CMutex recv_mutex_;
	CMutex send_mutex_;
};

#endif /*SESSION_BASE_H*/
