
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

class ServantProxy
{
public:
	ServantProxy();
	~ServantProxy();

	void setTimeout(int msec);
	int getTimeout();

	int invoke();

	

private:
	int _timeout_msec;

};

#endif //SERVANT_PROXY_H
