#include "resp_processor.h"
#include "response_manager.h"
#include "session_manager.h"
#include "client_manager.h"
#include "epoll_server.h"

#include "common/logger.h"
#include "common/index.h"
#include "common/utility.h"

RespProcessor::RespProcessor(SessionManager* pSessMgr, 
	ClientManager* pClientMgr) :_sess_mgr_prt(pSessMgr),
	_client_mgr_prt(pClientMgr)
{
}

RespProcessor::~RespProcessor()
{

}

/**
* brief:
*
* @returns   
*/
void RespProcessor::doIt()
{
	char buffer[16*1024] = {'\0'};

	ResponseManager* pRespMgr = ResponseManager::getInstance();
	assert(NULL != pRespMgr);
	
	while(!_terminate)
	{
		CmdTask resp;
		bool ret = pRespMgr->getRespTask(resp);
		if(!ret)
		{
			LOG4CPLUS_WARN(FLogger, "response array is empty.");
			continue;
		}

		SessionBasePtr pSession = _sess_mgr_prt->getSession(resp.seqno);

		if(!pSession)
		{
			LOG4CPLUS_ERROR(FLogger, "can't find the session by " << resp.seqno);

			_sess_mgr_prt->delSession(resp.seqno);

			continue;
		}

		int64_t uid = resp.pCmd->get_userid();
		string name = resp.pCmd->get_cmd_name();
		
		memset(buffer, '\0', 16 * 1024);
		int length = resp.pCmd->header_length() + resp.pCmd->body_length();	
		ret = resp.pCmd->encode((byte*)buffer, length);
	
		if(!ret)
		{
			LOG4CPLUS_ERROR(FLogger, "command encode failed for " << name);

			continue;
		}
		
		pSession->write2Send(string(buffer, length));
		int iret = pSession->sendBuffer();
		uint64_t data = U64(resp.seqno, pSession->getFd());

		if(SOCKET_EAGAIN == iret)	//socket������д��
		{	
			_epoll_svr_ptr->notify(pSession->getFd(), data, EVENT_WRITE);
		}
		else if(SOCKET_ERR == iret) //socket ����
		{
			_epoll_svr_ptr->notify(pSession->getFd(), data, EVENT_ERROR);

			_sess_mgr_prt->delSession(pSession);
			_client_mgr_prt->freeClient(uid, resp.seqno);
		}
		
		LOG4CPLUS_INFO(FLogger, "TimeTace: request[" << resp.idx << "] to response["
				<< name << "] spend time " << current_time_usec() - resp.timestamp);
	}
}
