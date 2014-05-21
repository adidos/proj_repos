#include "command_collector.h"

#include "gc_logger.h"

CmdCollector::CmdCollector(SessionManager* pSession)
	:session_mgr_ptr_(pSession), notify_array_(10240)
{
	queue_full_wait_ms_ = 200;
	queue_empty_wait_ms_ = 30000;
}

bool CmdCollector::collect(NotifyInfo& notify)
{
	return notify_array_.push(notify, queue_full_wait_ms_);	
}

void CmdCollector::doIt()
{
	while(!terminate_)
	{
		NotifyInfo notify;
		if(!notify_array_.pop(notify_array_, queue_empty_wait_ms_))
		{
			LOG4CPLUS_INFO(GCLogger::ROOT, "notify queue is empty!");
			continue;
		}
		
		handleNotify(notify);
	}
}


void CmdCollector::handleNotify(NotifyInfo& notify)
{
	if(!notify.isValid())	
	{
		LOG4CPLUS_WARN(GCLogger::ROOT, "invalid notify info, " << notify.dump());
		return;
	}

	assert(NULL != session_mgr_ptr_);
	SessionBase* pSession = session_mgr_ptr_->getSession(notify.seqno);	
	if(NULL == pSession)
	{
		LOG4CPLUS_WARN(GCLogger::ROOT, "session[" << notify.seqno << "] is NULL!");
		return ;
	}
	
	if(NT_CLOSE == notify.type)
	{
		LOG4CPLUS_INFO(GCLogger::ROOT, "socket close, fd = " << pSession->getFd());

		pSession->close();		
		session_mgr_ptr_->delSession(notify.seqno);		
		session_mgr_ptr_->pushBack(pSession);

		//TODO create a user drop command
	}
	else if(NT_READ == notify.type)
	{
		//TODO protocol parse
	}
	else
	{
		LOG4CPLUS_ERROR(GCLogger::ROOT, "error type: " << notify.type);
	}
}
