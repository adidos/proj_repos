
/* ======================================================
* 
* file:		servant.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-27 21:33:35
* 
* ======================================================*/

#ifndef GC_SERVANT_H
#define GC_SERVANT_H

class Servant
{
public:
	Servant();
	~Servant();
	
	int init();

	int startService();

	bool insertRecvQueue(DataXCmd* pCmd);
	bool insertSendQueue(DataXCmd* pCmd);

	bool getRecvCommand(DataXCmd* pCmd);
	bool getSendCommand(DataXCmd* pCmd);

private:
	//application
	SessionManager* _session_mgr_ptr;
	AcceptServer* _accept_server_ptr;
	EpollServer* _epoll_server_ptr;
	//CmdSendThread

	GCQueue _recv_queue;
	GCQueue _send_queue;

};

#endif //GC_SERVANT_H
