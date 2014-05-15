
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

#define MAX_BUF_LEN 8192

#include <string>

#define INVALID_FD -1
#define SEQNO_INIT 0

using namespace std;

class Command;

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

	int recv();
	int send();
	int close();

	int write2Send(const string& buffer_send);	
	
	/**
	* brief: get a complete command from recv_buff
	*
	* @returns  NULL or the command ptr 
	*/
	//Command* getCommand() = 0;

protected:
	int fd_;
	int seq_no_;

	string send_buff_;
	string recv_buff_;
};

#endif /*SESSION_BASE_H*/
