
/* ======================================================
* 
* file:		gc_thread.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-11 00:00:56
* 
* ======================================================*/
#include "gc_thread.h"

#include <cstdio>
#include <exception>

int GCThread::start()
{
	int ret = pthread_create(&tid_, NULL, threadEntry, (void*)this);
	if(0 != ret)
	{
		perror("pthread_create");
		return -1;
	}

	return 0;	
}

int GCThread::wait4Stop()
{
	return pthread_join(tid_, NULL);
}

void* GCThread::threadEntry(void* pParam)
{
	GCThread* thiz = (GCThread*)pParam;
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
		e.what();
	}

	return NULL;
}
