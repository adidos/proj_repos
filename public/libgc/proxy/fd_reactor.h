
/* ======================================================
* 
* file:		fd_reactor.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-15 23:12:28
* 
* ======================================================*/

#ifndef FD_REACTOR_H
#define FD_REACTOR_H

#include <map>

#include <stdint.h>

#include "server/epoll.h"
#include "common/thread.h"
#include "common/thread_sync.h"

using namespace std;

class CMutex;
class TransceiverHandle;

class FDReactor : public CThread
{
public:
	/**
	* constructor
	*/
	FDReactor();

	/**
	* destructor
	*/
	virtual ~FDReactor();

	
	/**
	* brief: register fd handle
	*
	* @param fd
	* @param event
	* @param pHanndler
	*/
	void regHandle(int fd, uint32_t event, TransceiverHandle* pHandle);

	/**
	* brief: cancel fd handle
	*
	* @param fd
	* @param event
	* @param pHanndle
	*/
	void unregHandle(int fd, uint32_t event, TransceiverHandle* pHandle);

	
	/**
	* brief:
	*
	* @param fd
	*/
	void notify(int fd);

protected:

	/**
	* thread work function
	*/
	virtual void doIt();

	/**
	* brief:
	*
	* @param fd
	* @param events
	*/
	void handle(int fd, int events);

private:
	bool _terminate;

	CEpoll _ep;

	map<int, TransceiverHandle*> _handles;

	CMutex _mutex;

};

#endif //FD_REACTOR_H
