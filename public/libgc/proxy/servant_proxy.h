
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

#include <string>

#include "adapter_proxy.h"

class DataXCmd;

class ServantProxy
{
public:
	ServantProxy();
	~ServantProxy();

	void setTimeout(int msec){_timeout_msec = msec;}

	int getTimeout(){return _timeout_msec;}

	string getName(){return _name;};

	void setName(const string& name){_name = name;};
	
	void regAdapterProxy(AdapterProxy* pAdapter)
	{
		_adapter_proxy = pAdapter;
	}

	int finished(ReqMessage* req);

protected:
	int invoke(DataXCmd* pReq, DataXCmd** pResp);

private:
	int _timeout_msec;

	string _name;

	AdapterProxy* _adapter_proxy;

};

#endif //SERVANT_PROXY_H
