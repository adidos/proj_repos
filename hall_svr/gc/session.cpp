
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

SessionBase::SessionBase()
	:fd_(-1), seq_no_(-1)
{

}

SessionBase::SessionBase(int fd, int seqno)
	:fd_(fd), seq_no_(seqno)
{
}

int SessionBase::recv()
{
	return 0;
}

int SessionBase::send()
{

}

int SessionBase::close()
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



