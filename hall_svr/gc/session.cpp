
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
#include "gc_logger.h"

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
	if(-1 == fd_)
	{
		LOG4CPLUS_ERROR(GCLogger::ROOT, "socket is invalide, close it!");
		return SOCKET_CLOSE;
	}

	char buffer[8192] = {'\0'};

	int total_len = 0;
	while(1)
	{
		memset(buffer, '\0', 8192);
		int recv_len = recv(fd_, buffer, 8192, 0);
		if(recv_len < 0)
		{
			if(EAGAIN == errno || EWOULDBLOCK == errno)
				return SOCKET_RECV_OVER;
			else if(EINTR == errno)
				continue;

			LOG4CPLUS_ERROR(GCLogger::ROOT, fd << " recv error, msg: " << strerror(errno));
			
			return SOCKET_ERR;
		}
		else if(recv_len == 0)
		{
			LOG4CPLUS_DEBUG(GCLogger::ROOT, fd << " close by client!");
			return SOCKET_CLOSE;
		}
		else
		{
			total_len += recv_len;

			CScopeGuard guard(recv_mutex_);
			recv_buff_.append(buffer, recv_len);
		}
	}

	LOG4CPLUS_TRACE(GCLogger::ROOT, fd << " recieve " << total_len << " bytes buffer!");
	return total_len;
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

