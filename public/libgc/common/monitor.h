
/* ======================================================
* 
* file:		monitor.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-18 19:52:20
* 
* ======================================================*/
#ifndef THREAD_MONITOR_H
#define THREAD_MONITOR_H

#include <pthread.h>

class Monitor
{
public:
	
	/**
	* brief:
	*/
	Monitor();
	
	/**
	* brief:
	*/
	~Monitor();

	/**
	* brief:
	*
	* @returns   
	*/
	int wait();

	/**
	* brief:
	*
	* @returns   
	*/
	int timewait(int msec);

	/**
	* brief:
	*
	* @returns   
	*/
	int notify();

private:
	int getAbsTimeout(int msec, struct timespec& abstimeout);

private:
	pthread_mutex_t _lock;
	pthread_cond_t	_cond;

};

#endif //THREAD_MONITOR_H
