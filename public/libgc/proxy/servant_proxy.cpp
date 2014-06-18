#include "servant_proxy.h"

#include "common/DataXCmd.h"
#include "common/index.h"


int ServantProxy::invoke(DataXCmd* pReq, DataXCmd** pResp)
{
	ReqMessage req = new ReqMessage;
	req->req_id = Index::get();
	req->type = SYNC_CALL;
	req->timeout = _timeout_msec;
	req->req = pReq;
	
	gettimeofday(&req->timestamp, NULL);

	invoke(req, resp);
		
	*pResp = req->resp;

	return 0;
}


int ServantProxy::invoke(ReqMessage* pReq)
{
	_adapter_proxy->invoke(pReq);

	return 0;
}