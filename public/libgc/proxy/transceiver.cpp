#include "transceiver.h"

Transceiver::Transceiver(int fd, bool connect)
	:_fd(fd), _connected(connect)
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

}

void Transceiver::writeToSendBuffer(const  string & msg)
{
	_send_buffer.append(msg);
}


////////////////////////////////////////////////////////////

int TcpTransceiver::doResponse(/*list<resp>*/>)
{
	if(!isValid()) return -1;

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

	//TODO protocol handle

	return 0;
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