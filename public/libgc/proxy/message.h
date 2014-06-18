
/* ======================================================
* 
* file:		message.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-16 22:45:59
* 
* ======================================================*/

#include <stdint.h>

#include <sys/time.h>

#include <string>

using namespace std;

struct ReqMessage
{
	uint32_t req_id;
	int type;
	timeval timeout;
	DataXCmd* req;
	DataXCmd* resp;
};

struct RespMessage
{


};
