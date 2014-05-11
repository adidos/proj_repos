
/* ======================================================
* 
* file:		gc_thread.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-10 23:58:24
* 
* ======================================================*/
#ifndef GC_THREAD_H
#define GC_THREAD_H

#include <pthread.h>

class GCThread
{
public:
	GCThread(){};
	~GCThread(){};

	int start();

	int wait4Stop();

protected:
	virtual void doIt() = 0;

private:
	static void* threadEntry(void* pParam);

private:
	pthread_t tid_;
};

#endif /*GC_THREAD_H*/
