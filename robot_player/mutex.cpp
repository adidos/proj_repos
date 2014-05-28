
/* ======================================================
* 
* file:		mutex.c
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-24 15:46:36
* 
* ======================================================*/

#include "logger.h"
#include <string.h>
#include <errno.h>

CMutex::CMutex()
{
	pthread_mutex_init(&_mutex, NULL);
}

CMutex::~CMutex()
{
	pthread_mutex_unlock(&_mutex);
	pthread_mutex_detroy(&_mutex);
}

/**
* brief:
*
* @returns   
*/
int CMutex::lock()
{
	int ret = pthread_mutex_lock(&_mutex);
	if(0 != ret)
	{
		LOG4CPLUS_ERROR(CDebugLogger::logger, "pthread_mutex_lock" << strerror(errno));
		return -1;
	}
	return 0;
}

/**
* brief:
*
* @returns   
*/
int CMutex::trylock()
{
	int ret = pthread_mutex_trylock(&_mutex);
	if(0 != ret)
	{
		LOG4CPLUS_ERROR(CDebugLogger::logger, "pthread_mutex_trylock" << strerror(errno));
		return -1;
	}

	return 0;
}

/**
* brief:
*
* @returns   
*/
int CMutex::unlock()
{
	int ret = pthread_mutex_unlock(&_mutex);
	if(0 != ret)
	{
		LOG4CPLUS_ERROR(CDebugLogger::logger, "pthread_mutex_unlock" << strerror(errno));
		return -1;
	}

	return 0;
}

