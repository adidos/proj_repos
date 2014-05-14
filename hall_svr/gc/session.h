
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

struct SessID
{
	int fd;
	int seqno;
	
	SessID()
	{
		fd = INVALID_FD;
		seqno = SEQNO_INIT;
	}

	SessID(const SessID& other)
	{
		this->fd = other.fd;
		this->seqno = other.seqno;
	}

	SessID& operator=(const SessID& other)
	{
		this->fd = other.fd;
		this->seqno = other.seqno;
		return *this;
	}

	bool operator < (const SessID& other)const
	{
		if(fd < other.fd)
			return true;
		else if(fd == other.fd)
			return seqno < other.seqno;
		
		return false;
	}
};

class SessionBase
{
public:
	SessionBase(const SessID& sid);
	~SessionBase();
	
	void setSessId(const SessID& id)
	{
		sid_ = id;	
	}

	void reset();

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
	SessID sid_;

	string send_buff_;
	string recv_buff_;
};

#endif /*SESSION_BASE_H*/
