
/* ======================================================
* 
* file:		resp_processor.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-09 11:36:07
* 
* ======================================================*/

#ifndef RESP_PROCESSOR_H
#define RESP_PROCESSOR_H

#include "common/queue.h"
#include "common/thread.h"

#include "session_manager.h"
#include "client_manager.h"
#include "epoll_server.h"

class RespProcessor : public CThread
{
public:
	RespProcessor(SessionManager* pSessMgr, ClientManager* pClientMgr);
	~RespProcessor();
	
	int regEventServer(EpollServer* pEpollSvr)
	{
		_epoll_svr_ptr = pEpollSvr;
	}

protected:
	virtual void doIt();

private:
	SessionManager* _sess_mgr_prt;
	ClientManager* _client_mgr_prt;
	EpollServer* _epoll_svr_ptr;

};

#endif //RESP_PROCESSOR_H
