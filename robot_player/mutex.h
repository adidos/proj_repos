
/* ======================================================
* 
* file:		mutex.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-24 15:22:36
* 
* ======================================================*/

#ifndef DOS_MUTEX_H
#define DOS_MUTEX_H

#include <pthread.h>

class CMutex
{
public:
	CMutex();
	~CMutex();

	int lock();
	int trylock();
	int unlock();

private:
	pthread_mutex_t _mutex;

};

class CGuard
{
public:
	CGuard(CMutex& mutex)
	{
		_mutex_ptr = &mutex;
		mutex.lock();
	}

	~CGuard()
	{
		_mutex_ptr->unlock();
	}

private:
	CMutex* _mutex_ptr;
};

#endif //DOS_MUTEX_H
