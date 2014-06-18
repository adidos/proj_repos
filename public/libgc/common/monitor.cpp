
/* ======================================================
* 
* file:		monitor.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-18 19:59:25
* 
* ======================================================*/

#include "monitor.h"

#include <sys/time.h>

Monitor::Monitor()
{
	pthread_mutex_init(&_lock,NULL);

	pthread_cond_init(&_cond, NULL);

}

Monitor::~Monitor()
{
	pthread_mutex_destroy(&_lock);	

	pthread_cond_destroy(&_cond);
}

int Monitor::wait()
{
	pthread_cond_wait(&_cond, &_lock);
	
	return 0;
}

int Monitor::timewait(int msec)
{
	struct timespec timespot;
	getAbsTimeout(msec, timespot);

	pthread_cond_timedwait(&_cond, &_lock, &timespot);

	return 0;
}

int Monitor::notify()
{
	pthread_cond_signal(&_cond);
	return 0;
}

int Monitor::getAbsTimeout(int msec, struct timespec& abstimeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	msec += (now.tv_usec / 1000);

	abstimeout.tv_sec = now.tv_sec + msec / 1000;
	abstimeout.tv_nsec = (msec % 1000) * 1000 * 1000;

	return 0;
}

