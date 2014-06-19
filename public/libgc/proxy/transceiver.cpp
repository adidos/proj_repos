#include "transceiver.h"

#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common/utility.h"

Transceiver::Transceiver()
	:_fd(-1), _connected(false)
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

int Transceiver::close()
{
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
	if(!isValid()) return -1;

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
	}while(ret > 0)

	return ret;

}

void Transceiver::writeToSendBuffer(const  string & msg)
{
	_send_buffer.append(msg);
}


////////////////////////////////////////////////////////////

int TcpTransceiver::doConnect()
{
	if(_fd >0)	close(_fd);
	
	_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	
	if(_fd < 0)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "socket error:" << strerror(errno));
		return -1;
	}

	setNoBlock(_fd);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host.c_str());

	bool bConnect = false;
	int ret = ::connect(fd, (sockaddr*)&addr, (socklen_t)sizeof(addr));
	if(0 == ret)
	{
		_connected = true;
	}
	else if(ret < 0 && EINPROGRESS != errno)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "connect error:" << strerror(errno));
		close(fd);
		return -1;
	}

	return 0;

}

int TcpTransceiver::doResponse(list<DataXCmd*>& resps)
{
	if(!isValid()) return -1;

	resps.clear();

	int recv = 0;

	char buffer[8192] = {'\0'};
	
	do
	{
		if((recv = recv(buffer, sizeof(buffer), 0)) > 0)
		{
			_recv_buffer.append(buffer, recv);
			memset(buffer, '\0', 8192);
		}
	} while (recv > 0)

	if(_recv_buffer.empty()) return -1;

	//TODO make abstract interface for the protocol decode

	int index = 0;
	
	do
	{
		DataXCmd* pCmd = new DataXCmd();
		
		int header = pCmd->header_length();
		
		if(header > _recv_buffer.length())
		{
			delete pCmd; pCmd = NULL;
			
			break;
		}

		byte* ptr = (byte*)(_recv_buffer.c_str() + index);
		pCmd->decode_header(ptr, header);

		int body = pCmd->body_length();

		if(body + header > _recv_buffer.length())
		{
			delete pCmd; pCmd = NULL;

			break;
		}

		ptr += header;
		pCmd->decode_parameters(ptr, body);

		resps.push_back(pCmd);

		index += (body + header);
	}while(1)

	_recv_buffer.erase(0, index);	

	return resps.size();
}

int TcpTransceiver::send(const void* buf, uint32_t len, uint32_t flag)
{
	if(!isValid()) return -1;

    int ret = ::send(_fd, buf, len, flag);

    if (ret < 0 && errno != EAGAIN)
    {
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