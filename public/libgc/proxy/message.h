
/* ======================================================
* 
* file:		message.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-16 22:45:59
* 
* ======================================================*/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#include <sys/time.h>

#include <string>

#include "common/DataXCmd.h"
#include "common/monitor.h"

#include "servant_proxy.h"

using namespace std;

enum 
{
	SYNC_CALL = 0x1,
	ASYNC_CALL = 0x2
};

struct ReqMessage
{
	uint32_t id;
	int type;
	timeval stamp;
	ServantProxy* proxy;
	DataXCmd* req;
	DataXCmd* resp;
	Monitor* monitor;

	ReqMessage(){};

	~ReqMessage()	//resp 不释放，在子类中取出数据后释放
	{
		if(NULL != req)
		{
			delete req;
			req = NULL;
		}

		if(NULL != monitor)
		{
			delete monitor;
			monitor = NULL;
		}
	}
};

struct RespMessage
{
	uint32_t req_id;
	DataXCmd* resp;
};

#endif
