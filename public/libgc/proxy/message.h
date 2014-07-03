

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

#include <memory>
#include <string>

#include "common/DataXCmd.h"
#include "common/monitor.h"

class ServantProxy;

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
	DataXCmdPtr req;
	DataXCmdPtr resp;
	MonitorPtr monitor;

	ReqMessage(){};
};

struct RespMessage
{
	uint32_t req_id;
	DataXCmdPtr resp;
};

typedef shared_ptr<ReqMessage> ReqMessagePtr;

#endif
