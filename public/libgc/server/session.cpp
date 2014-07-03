
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
#include "common/logger.h"

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

SessionBase::SessionBase()
	:fd_(-1), seq_no_(-1)
{

}

SessionBase::SessionBase(int fd, int seqno)
	:fd_(fd), seq_no_(seqno)
{
}

void SessionBase::clearBuffer()
{
	{
		CScopeGuard guard(recv_mutex_);
		recv_buff_.clear();
	}

	{
		CScopeGuard guard(send_mutex_);
		send_buff_.clear();
	}
}
int SessionBase::recvBuffer()
{
	if(-1 == fd_)
	{
		LOG4CPLUS_ERROR(FLogger, "socket is invalide, close it!");
		return SOCKET_CLOSE;
	}

	char buffer[8192] = {'\0'};

	int total_len = 0;
	while(1)
	{
		memset(buffer, '\0', 8192);
		int recv_len = ::recv(fd_, buffer, 8192, 0);
		if(recv_len < 0)
		{
			if(EAGAIN == errno || EWOULDBLOCK == errno)
			{
				LOG4CPLUS_TRACE(FLogger, fd_ << " will recv EAGAIN, error:" << strerror(errno));
				break;
			}
			else if(EINTR == errno)
				continue;

			LOG4CPLUS_ERROR(FLogger, fd_ << " recv error, msg: " << strerror(errno));
			
			return SOCKET_ERR;
		}
		else if(recv_len == 0)
		{
			LOG4CPLUS_DEBUG(FLogger, fd_ << " close by client!");
			return SOCKET_CLOSE;
		}
		else
		{
			total_len += recv_len;

			CScopeGuard guard(recv_mutex_);
			recv_buff_.append(buffer, recv_len);
		}
	}

	LOG4CPLUS_TRACE(FLogger, fd_ << " recieve " << total_len << " bytes buffer!");
	return total_len;
}

int SessionBase::sendBuffer()
{
	uint32_t total_len = 0;
	CScopeGuard gaurd(send_mutex_);
	
	while(1)
	{
		if(-1 == fd_ ) 
		{
			LOG4CPLUS_ERROR(FLogger, "socket fd = -1!");
			return SOCKET_ERR;
		}
		
		if(send_buff_.empty())return 0;
		
		int send_len = ::send(fd_, send_buff_.c_str() + total_len, send_buff_.size(), 0);
		if(send_len < 0)
		{
			if(EWOULDBLOCK == errno || EAGAIN == errno)
			{
				LOG4CPLUS_WARN(FLogger, fd_ << " will recv EAGAIN, error:" 
						<< strerror(errno));

				send_buff_.erase(0, total_len);
				return SOCKET_EAGAIN;
			}
			else if(EINTR == errno)
			{
				continue;
			}

			LOG4CPLUS_ERROR(FLogger, fd_ << " send error, msg: " << strerror(errno));

			return SOCKET_ERR;
		}
		else if(send_len >= 0)
		{
			total_len += send_len;
			if(total_len == send_buff_.size())
				break;
		}		
	}
	send_buff_.erase(0, total_len);

	return total_len;	
}

int SessionBase::close()
{
	::close(fd_);
	fd_ = -1;
	
	clearBuffer();
	return 0;
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
	CScopeGuard guard(send_mutex_);
	send_buff_.append(buffer_send);
	
	return 0;
}

int SessionBase::parseProtocol(DataXCmdPtr &pCmd)
{
	DataXCmdPtr ptr(new DataXCmd());

	CScopeGuard gaurd(recv_mutex_);
	unsigned int header = ptr->header_length();
	if(recv_buff_.length() < header)
	{
		LOG4CPLUS_DEBUG(FLogger, "recv buffer length is " << recv_buff_.length()
			<< ", is less then header require length " << header);

		return -1;
	}
	
	bool ret = ptr->decode_header((byte*)recv_buff_.c_str(), header);	
	if(!ret)
	{
		return -1;
	}

	unsigned int body = ptr->body_length();
	if(recv_buff_.length() < body + header)
	{
		LOG4CPLUS_DEBUG(FLogger, "recv buffer length is " << recv_buff_.length()
			<<", is less then a complete command package length " << body + header);

		return -1;
	}
	
	byte* pBody = (byte*)(recv_buff_.c_str() + header);
	ret = ptr->decode_parameters(pBody, body);
	if(!ret)
	{
		LOG4CPLUS_DEBUG(FLogger, "DataXCmd decode parameters failed!");
		return -1;
	}

	LOG4CPLUS_DEBUG(FLogger, "pcmd[" << ptr->get_cmd_name() << "] reference is " << ptr.use_count());
	pCmd = ptr;
	LOG4CPLUS_DEBUG(FLogger, "pcmd[" << ptr->get_cmd_name() << "] reference is " << ptr.use_count());

	//delete the decode buffer from recv buffer
	recv_buff_.erase(0, body + header);

	return 0;
}
