
/* ======================================================
* 
* file:		notice_config_thread.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-22 15:10:05
* 
* ======================================================*/

#ifndef NOTICE_CONFIG_THREAD_H
#define NOTICE_CONFIG_THREAD_H

#include "notice_config.h"
#include "SDLogger.h"
#include "SDThreadPool.h"
#include "ThreadSync.h"
#include "redis_handler.h"

#include <string>

using namespace std;

class NoticeConfigThread : public SDThreadPool
{
public:
	NoticeConfigThread(const string& redis_svr, short port);

	int init(const string& redis_svr, short port);
	
protected:
	virtual void doIt();

private:
	int parseConfig(const string& config, vector<Notice>& out_config);

private:
	RedisHandler* redis_handler_ptr_;

	CMutex mutex_;

	DECL_LOGGER(logger);
};

#endif /*NOTICE_CONFIG_THREAD_H*/
