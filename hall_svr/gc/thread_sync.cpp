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
#include "gc_logger.h"

#include <assert.h>

CMutex::CMutex()
{
	memset(&m_mutex, 0, sizeof(pthread_mutex_t));
	Init();
}

CMutex::~CMutex()
{
	pthread_mutex_unlock(&m_mutex);
	pthread_mutex_destroy(&m_mutex);
	pthread_mutexattr_destroy(&m_attr);
}

/**
* brief:
*
* @returns   
*/
int CMutex::Init()
{
	if (pthread_mutexattr_init(&m_attr) != 0)
	{
	    LOG4CPLUS_ERROR(GCLogger::ROOT, "pthread_mutexattr_init err: " << strerr(errno));
		return -1;
	}

	if (pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE) != 0)
	{
	    LOG4CPLUS_ERROR(GCLogger::ROOT, "pthread_mutexattr_settype err: " << strerr(errno));
		return -1;
	}

	if (pthread_mutex_init(&m_mutex, &m_attr) != 0)
	{
	    LOG4CPLUS_ERROR(GCLogger::ROOT, "pthread_mutex_init err: " << strerr(errno));
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
	    LOG4CPLUS_ERROR(GCLogger::ROOT, "pthread_mutex_lock err: " << strerr(errno));
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
	    LOG4CPLUS_ERROR(GCLogger::ROOT, "pthread_mutex_trylock err: " << strerr(errno));
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
	    LOG4CPLUS_ERROR(GCLogger::ROOT, "pthread_mutex_unlock err: " << strerr(errno));
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
	if (m_pMutex->Lock())
	{
		m_bLocked = 1;
	}
	else
	{
		m_bLocked = 0;
	}
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
	if (m_bLocked)
	{
		if (m_pMutex->Unlock())
		{
			m_bLocked = 0;
		}
	}
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
	if (m_pMutex->Lock())
	{
		m_bLocked = 1;
		return 1;
	}
	else
	{
		m_bLocked = 0;
		return 0;
	}
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
	if (m_bLocked)
	{
		if (m_pMutex->Unlock())
		{
			m_bLocked = 0;
			return 1;
		}
		return 0;
	}
	
	return 1;
}

