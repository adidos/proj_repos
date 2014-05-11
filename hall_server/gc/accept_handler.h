
/* ======================================================
* 
* file:		accept_handler.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-11 15:46:15
* 
* ======================================================*/

#ifndef ACCEPT_HANDLER_H
#define ACCEPT_HANDLER_H

#include "event_handler.h"

class AcceptHandler : public IEventHandler
{
public:
	AcceptHandler();
	~AcceptHandler();

	virtual void handle(int fd, int event);
	
	virtual void handleError(int fd);




};

#endif /*ACCEPT_HANDLER_H*/
