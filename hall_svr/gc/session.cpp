
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
#include "logger.h"

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
		LOG4CPLUS_ERROR(CLogger::logger, "socket is invalide, close it!");
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
			{
				LOG4CPLUS_TRACE(CLogger::logger, fd << " will recv EAGAIN");
				break;
			}
			else if(EINTR == errno)
				continue;

			LOG4CPLUS_ERROR(CLogger::logger, fd << " recv error, msg: " << strerror(errno));
			
			return SOCKET_ERR;
		}
		else if(recv_len == 0)
		{
			LOG4CPLUS_DEBUG(CLogger::logger, fd << " close by client!");
			return SOCKET_CLOSE;
		}
		else
		{
			total_len += recv_len;

			CScopeGuard guard(recv_mutex_);
			recv_buff_.append(buffer, recv_len);
		}
	}

	LOG4CPLUS_TRACE(CLogger::logger, fd << " recieve " << total_len << " bytes buffer!");
	return total_len;
}

int SessionBase::send()
{

}

int SessionBase::close()
{
	close(fd_);
	fd_ = -1;
	
	{
		CScopeGuard guard(recv_mutex_);
		recv_buff_.clear();
	}

	{
		CScopeGuard guard(send_mutex_);
		send_buff_.clear();
	}
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

int SessionBase::parseProtocol(DataXCmd* pCmd)
{
	DataXCmd* ptr;
	CScopeGuard gaurd(recv_mutex_);
	int header = ptr->header_length();
	if(recv_buff_.length() < header)
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "recv buffer length is " << recv_buff_.length()
			<< ", is less then header require length " << header);
		cmdRelease(ptr);

		return -1;
	}
	
	bool ret = pCmd->decode_header((byte*)recv_buff_.c_str(), header);	
	if(!ret)
	{
		cmdRelease(ptr);
		
		return -1;
	}

	int body = pCmd->body_length();
	if(recv_buff_.length() < body + header)
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "recv buffer length is " << recv_buff_.length()
			<<", is less then a complete command package length " << body + header);

		cmdRelease(ptr);

		return -1;
	}
	
	byte* pBody = (byte*)(recv_buff_.c_str() + header);
	ret = pCmd->decode_parameters(pBody, body);
	if(!ret)
	{
		cmdRelease(ptr);
		return -1;
	}

	pCmd = ptr;
	//delete the decode buffer from recv buffer
	recv_buff_.erase(0, body + header);

	return 0;
}
