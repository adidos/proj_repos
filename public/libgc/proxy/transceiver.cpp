#include "transceiver.h"

#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common/utility.h"
#include "common/logger.h"
#include "common/DataXCmd.h"
#include "common/logger.h"
#include "common/function_trace.h"

Transceiver::Transceiver(int fd, bool bConnect)
	:_fd(fd), _connected(bConnect)
{

}

Transceiver::~Transceiver()
{
	close();
}

int Transceiver::fd() const
{
	return _fd;
}

void Transceiver::close()
{
	LOG4CPLUS_DEBUG(FLogger, "socket close...");
	::close(_fd);
	_connected = false;
	_fd = -1;
}

bool Transceiver::isValid() const
{
    return (_fd != -1);
}

int Transceiver::doRequest()
{
	if(!isValid()) 
	{
		LOG4CPLUS_ERROR(FLogger, "socket is invalid!");
	
		return -1;
	}

	int ret = 0;
	
	do
	{
		ret = 0;

		if(!_send_buffer.empty())
		{
			size_t len = _send_buffer.length();

			ret = this->send(_send_buffer.c_str(), _send_buffer.length(), 0);
			if(ret > 0)
			{
				LOG4CPLUS_DEBUG(FLogger, "success send " << ret  << " bytes!");
				if((size_t) ret == len)
				{
					_send_buffer.clear();
				}
				else
				{
					_send_buffer.erase(_send_buffer.begin(), _send_buffer.begin() + ret);
				}
			}
		}
	}while(ret > 0);

	return ret;

}

void Transceiver::writeToSendBuffer(const  string & msg)
{
	_send_buffer.append(msg);
	LOG4CPLUS_DEBUG(FLogger, "after append, send length is " << _send_buffer.size());
}


////////////////////////////////////////////////////////////


TcpTransceiver::TcpTransceiver(int fd, bool bConnect)
	:Transceiver(fd, bConnect)
{
}

int TcpTransceiver::doConnect(const string& host, short port)
{
	_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	
	if(_fd < 0)
	{
		LOG4CPLUS_ERROR(FLogger, "socket error:" << strerror(errno));
	}

	setNoBlock(_fd);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host.c_str());

	int ret = ::connect(_fd, (sockaddr*)&addr, (socklen_t)sizeof(addr));

	int err_no = errno;

	LOG4CPLUS_DEBUG(FLogger, "connect to server " << host 
				<< ":" << port << " error: " << strerror(err_no));

	if(0 == ret)
	{
		_connected = true;
	}
	else if(ret < 0 && EINPROGRESS != err_no)
	{
		LOG4CPLUS_ERROR(FLogger, "connect error:" << strerror(err_no));
		::close(_fd);
		return -1;
	}

	return 0;

}

int TcpTransceiver::doResponse(list<DataXCmdPtr>& resps)
{
	if(!isValid()) 
	{
		LOG4CPLUS_ERROR(FLogger, "invalid socket fd!");

		return -1;
	}

	resps.clear();

	int len = 0;

	char buffer[8192] = {'\0'};
	
	do
	{
		if((len= this->recv(buffer, sizeof(buffer), 0)) > 0)
		{
			_recv_buffer.append(buffer, len);
			memset(buffer, '\0', 8192);
		}
	} while (len > 0);

	LOG4CPLUS_DEBUG(FLogger, "recv buffer length is " << _recv_buffer.length());

	if(_recv_buffer.empty()) return -1;

	//TODO make abstract interface for the protocol decode

	size_t index = 0;

	do
	{
		DataXCmdPtr pCmd(new DataXCmd());
		
		unsigned int header = pCmd->header_length();

		LOG4CPLUS_DEBUG(FLogger, "header length is " << header);
		
		if(header > _recv_buffer.length() - index)
		{
			LOG4CPLUS_WARN(FLogger, "recv buffer length  less then header length");
			break;
		}

		byte* ptr = (byte*)(_recv_buffer.c_str() + index);
		pCmd->decode_header(ptr, header);

		uint32_t body = pCmd->body_length();

		LOG4CPLUS_DEBUG(FLogger, "body length is " << body);

		if(body + header > _recv_buffer.length() - index)
		{
			LOG4CPLUS_WARN(FLogger, "recv buffer length  less then body length");

			break;
		}

		ptr += header;

		if(! pCmd->decode_parameters(ptr, body)) break;

		index = index + header + body;

		resps.push_back(pCmd);

		LOG4CPLUS_DEBUG(FLogger, "pCmd use count is " << pCmd.use_count());

	}while(1);

	_recv_buffer.erase(0, index);	

	return resps.size();
}

int TcpTransceiver::send(const void* buf, uint32_t len, uint32_t flag)
{
	if(!isValid()) return -1;

    int ret = ::send(_fd, buf, len, flag);

    if (ret < 0 && errno != EAGAIN)
    {
		LOG4CPLUS_ERROR(FLogger, "send error: " << strerror(errno));
        close();

        return 0;
    }

    return ret;
}

int TcpTransceiver::recv(void* buf, uint32_t len, uint32_t flag)
{
    if(!isValid()) return -1;

    int ret = ::recv(_fd, buf, len, flag);

    if (ret == 0 || (ret < 0 && errno != EAGAIN))
    {
        close();

        return 0;
    }

    return ret;
}

