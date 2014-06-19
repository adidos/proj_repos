#include "servant_proxy.h"

#include "common/logger.h"
#include "common/DataXCmd.h"
#include "common/index.h"
#include "message.h"

int ServantProxy::invoke(DataXCmd* pReq, DataXCmd** pResp)
{
	ReqMessage* req = new ReqMessage;
	req->id = Index::get();
	req->type = SYNC_CALL;
	req->req = pReq;
	req->proxy = this;
	req->resp = NULL;
	
	gettimeofday(&req->stamp, NULL);

	int ret = _adapter_proxy->invoke(req);
	if(0 != ret)
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "Adapter proxy invoke failed! ");
		return ret;
	}

	if(SYNC_CALL == req->type)
	{
		req->monitor = new Monitor();
		req->monitor->timewait(_timeout_msec);

		*pResp = req->resp;
	}

	timeval now;
	gettimeofday(&now, NULL);

	int interval = (now.tv_sec - req->stamp.tv_sec)*1000 + (now.tv_usec - req->stamp.tv_usec)/1000;

	LOG4CPLUS_DEBUG(CLogger::logger, "invoke finished! spend time " << interval 
			<< ", is timeout("<< _timeout_msec << ") ? " << (interval > _timeout_msec));

	delete pReq;

	return 0;
}

int ServantProxy::finished(ReqMessage* req)
{
	//check whether the request is timeout
	if(NULL == req) return 0;

	req->monitor->notify();
	
	return 0;
}
