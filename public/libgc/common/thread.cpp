
/* ======================================================
* 
* file:		thread.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-11 00:00:56
* 
* ======================================================*/
#include "thread.h"

#include <cstdio>
#include <exception>

#include "logger.h"

int CThread::start()
{
	int ret = pthread_create(&tid_, NULL, threadEntry, (void*)this);
	if(0 != ret)
	{
		perror("pthread_create");
		return -1;
	}

	return 0;	
}

int CThread::waitForStop()
{
	return pthread_join(tid_, NULL);
}

void* CThread::threadEntry(void* pParam)
{
	CThread* thiz = (CThread*)pParam;
	if(NULL == thiz)
	{
		return NULL;
	}

	try
	{
		thiz->doIt();
	}
	catch(std::exception& e)
	{
		LOG4CPLUS_ERROR(FLogger, "exception: " <<e.what());
	}

	return NULL;
}
