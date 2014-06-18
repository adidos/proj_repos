
/* ======================================================
* 
* file:		servant_proxy.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-15 21:17:56
* 
* ======================================================*/
#ifndef SERVANT_PROXY_H
#define SERVANT_PROXY_H

#incude <string>

#include "message.h"

class DataXCmd;
class AdapterProxy;

class ServantProxy
{
public:
	ServantProxy();
	~ServantProxy();

	void setTimeout(int msec);
	int getTimeout();
	
	void regAdapterProxy(AdapterProxy* pAdapter)
	{
		_adapter_proxy = pAdapter;
	}

protected:
	int ServantProxy::invoke(DataXCmd* pReq, DataXCmd** pResp);

private:
	int invoke(ReqMessage* pReq);

	int encode_command(DataXCmd* pCmd, std::string& buffer);

private:
	int _timeout_msec;

	AdapterProxy* _adapter_proxy;

};

#endif //SERVANT_PROXY_H
