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
	    LOG4CPLUS_ERROR(CLogger::logger, "pthread_mutexattr_init err: " << strerror(errno));
		return -1;
	}

	if (pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE) != 0)
	{
	    LOG4CPLUS_ERROR(CLogger::logger, "pthread_mutexattr_settype err: " << strerror(errno));
		return -1;
	}

	if (pthread_mutex_init(&m_mutex, &m_attr) != 0)
	{
	    LOG4CPLUS_ERROR(CLogger::logger, "pthread_mutex_init err: " << strerror(errno));
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
	    LOG4CPLUS_ERROR(CLogger::logger, "pthread_mutex_lock err: " << strerror(errno));
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
	    LOG4CPLUS_ERROR(CLogger::logger, "pthread_mutex_trylock err: " << strerror(errno));
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
	    LOG4CPLUS_ERROR(CLogger::logger, "pthread_mutex_unlock err: " << strerror(errno));
		return -1;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//		  Class CScopeGuard,�򵥵ľֲ���Դ����
/******************************************************************************
������ :CScopeGuard
����   :���캯��,��ϵͳ��
����   :��
���   :��
����ֵ :��
����   :��
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
������ :~CScopeGuard
����   :�͹�����,��ϵͳ��
����   :��
���   :��
����ֵ :��
����   :��
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
������ :Lock
����   :��ϵͳ���������м���
����   :��
���   :��
����ֵ :�ɹ�����1,ʧ��0
����   :��
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
������ :Unlock
����   :��ϵͳ����������
����   :��
���   :��
����ֵ :�ɹ�����1,ʧ��0
����   :��
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

