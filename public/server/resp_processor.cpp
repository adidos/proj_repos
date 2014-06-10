#include "resp_processor.h"
#include "logger.h"
#include "index.h"
#include "response_manager.h"

RespProcessor::RespProcessor(SessionManager* pSessMgr, 
	ClientManager* pClientMgr) :_sess_mgr_prt(pSessMgr),
	_client_mgr_prt(pClientMgr), _resp_array(102400)
{
}

RespProcessor::~RespProcessor()
{

}

int RespProcessor::putResponse(CmdTask & resp)
{
	bool ret = _resp_array.push(resp, 50);
	if(!ret)
		LOG4CPLUS_ERROR(CLogger::logger, "response array is full!");

	return (ret ? 0 : -1)
}

int RespProcessor::doIt()
{
	char buffer[16*1024] = {'\0'};

	ResponseManager* pRespMgr = ResponseManager::getInstance();
	assert(NULL != pRespMgr);
	
	while(true)
	{
		CmdTask resp;
		bool ret = pRespMgr->getRespTask(resp);
		if(!ret)
		{
			LOG4CPLUS_WARN(CLogger::logger, "response array is empty, " << _resp_array.getSize());
			continue;
		}

		SessionBase* pSession = _sess_mgr_prt->getSession(resp.seqno);
		if(NULL == pSession)
		{
			LOG4CPLUS_ERROR(CLogger::logger, "can't find the session by " << resp.seqno);
			delete resp.pCmd;
			resp.pCmd = NULL;
			continue;
		}

		int64_t uid = resp.pCmd->get_userid();
		string name = resp.pCmd->get_cmd_name();
		
		memset(buffer, '\0', 16 * 1024);
		int length = resp.pCmd->header_length() + resp.pCmd->body_length();	
		ret = resp.pCmd->encode((byte*)buffer, length);

		resp.releaseCmd();
	
		if(!ret)
		{
			LOG4CPLUS_ERROR(CLogger::logger, "command encode failed for " << name);
			continue;
		}
		
		pSession->write2Send(string(buffer, length));
		int ret = pSession->sendBuffer();
		uint64_t data = U64(resp.seqno, pSession->getFd());
		if(SOCKET_EAGAIN == ret)	//socket»º³åÇøÐ´Âú
		{	
			_epoll_svr_ptr->notify(pSession->getFd(), data, EVENT_WRITE);
		}
		else if(SOCKET_ERR == ret) //socket ³ö´í
		{
			_epoll_svr_ptr->notify(pSession->getFd(), data, EVENT_ERROR);
			_sess_mgr_prt->freeSession(pSession);
			_client_mgr_prt->freeClient(uid, resp.seqno);
		}
		
		LOG4CPLUS_DEBUG(CDebugLogger::logger, "TimeTace: request[" << resp.idx << "] to response["
				<< name << "] spend time " << current_time_usec() - resp.timestamp);
		Index.free(resp.idx);
	}
}