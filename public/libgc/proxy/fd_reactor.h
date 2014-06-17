
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

using namespace std;

#include "common/thread.h"

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
	void regHandle(int fd, uint32_t event, TransceiverHandle* pHanndle);

	/**
	* brief: cancel fd handle
	*
	* @param fd
	* @param event
	* @param pHanndle
	*/
	void unregHandle(int fd, uint32_t event, TransceiverHandle* pHanndle);

	
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
