
/* ======================================================
* 
* file:		thread.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-10 23:58:24
* 
* ======================================================*/
#ifndef GC_THREAD_H
#define GC_THREAD_H

#include <pthread.h>

class CThread
{
public:
	CThread()
	{
		ostringstream oss;
		oss << showbase << hex << pthread_self();
		_id = oss.str();
	};
	
	~CThread(){};

	int start();

	int waitForStop();

protected:
	virtual void doIt() = 0;

protected:
	string _id;

private:
	static void* threadEntry(void* pParam);

private:
	pthread_t tid_;
};

#endif /*GC_THREAD_H*/
