/* ======================================================
* 
* file:		thread_sync.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-16 17:05:26
* 
* ======================================================*/

#include "thread_sync.h"
#include "logger.h"

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

CMutex::CMutex()
{
	memset(&m_mutex, 0, sizeof(pthread_mutex_t));
	Init();
}

CMutex::~CMutex()
{
	pthread_mutex_unlock(&m_mutex);
	pthread_mutex_destroy(&m_mutex);
}

/**
* brief:
*
* @returns   
*/
int CMutex::Init()
{
	if (pthread_mutex_init(&m_mutex, NULL) != 0)
	{
	    LOG4CPLUS_ERROR(FLogger, "pthread_mutex_init err: " << strerror(errno));
		return -1;
	}

	return 0;
}

/**
* brief:
*
* @returns   
*/
int CMutex::Lock()
{
	if (0 != pthread_mutex_lock(&m_mutex))
	{
	    LOG4CPLUS_ERROR(FLogger, "pthread_mutex_lock err: " << strerror(errno));
		return -1;
	}

	return 0;
}

/**
* brief:
*
* @returns   
*/
int CMutex::TryLock()
{
	if (0 != pthread_mutex_trylock(&m_mutex))
	{
	    LOG4CPLUS_ERROR(FLogger, "pthread_mutex_trylock err: " << strerror(errno));
		return -1;
	}

	return 0;
}

/**
* brief:
*
* @returns   
*/
int CMutex::Unlock()
{
	if (0 != pthread_mutex_unlock(&m_mutex))
	{
	    LOG4CPLUS_ERROR(FLogger, "pthread_mutex_unlock err: " << strerror(errno));
		return -1;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//		  Class CScopeGuard,简单的局部资源保护
/******************************************************************************
函数名 :CScopeGuard
功能   :构造函数,加系统锁
输入   :无
输出   :无
返回值 :无
其他   :无
******************************************************************************/
CScopeGuard::CScopeGuard(CMutex& mutex)
{

	m_pMutex = &mutex;
	m_pMutex->Lock();
	
}

/******************************************************************************
函数名 :~CScopeGuard
功能   :释构函数,解系统锁
输入   :无
输出   :无
返回值 :无
其他   :无
******************************************************************************/
CScopeGuard::~CScopeGuard()
{
	m_pMutex->Unlock();
}

/******************************************************************************
函数名 :Lock
功能   :对系统互斥量进行加锁
输入   :无
输出   :无
返回值 :成功返回1,失败0
其他   :无
******************************************************************************/
int CScopeGuard::Lock()
{
	m_pMutex->Lock();
	return 0;
}

/******************************************************************************
函数名 :Unlock
功能   :对系统互斥量解锁
输入   :无
输出   :无
返回值 :成功返回1,失败0
其他   :无
******************************************************************************/
int CScopeGuard::Unlock()
{
	m_pMutex->Unlock();
		
	return 0;
}

