/* ======================================================
* 
* file:		thread_sync.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-16 16:58:29
* 
* ======================================================*/
#ifndef THREAD_SYNC_H
#define THREAD_SYNC_H

#include <pthread.h>
#include <sys/sem.h>

class CMutex
{
public:
	CMutex();
	~CMutex();

	int Lock();
	int TryLock();
	int Unlock();

private:
	int Init();

private:
	pthread_mutex_t m_mutex;
	pthread_mutexattr_t m_attr;
};

class CScopeGuard
{
public:
	CScopeGuard(CMutex& mutex);
	~CScopeGuard();

	int Lock();

	int Unlock();
private:
	int 		m_bLocked;
	CMutex* m_pMutex;
};

#endif //THREAD_SYNC_H

