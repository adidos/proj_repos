
/* ======================================================
* 
* file:		session.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-12 22:23:23
* 
* ======================================================*/

#include "session.h"

SessionBase::SessionBase(const SessID& sid)
	:sid_(sid)
{
}

/**
* brief: set the command to the send_buff
*
* @param pCmd: the command want to send
*
* @returns  0 success  -1 the send buffer is full 
*/
int SessionBase::write2Send(const string& buffer_send)
{
	
	return 0;
}



