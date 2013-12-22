#include "socket.h"

#include <iostream>

using namespace std;

CSocket::CSocket() : fd_(INVALID_SOCKET)
{
}

int CSocket::init()
{
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET == fd_)
	{
		cout << "error: " << WSAGetLastError() <<endl;
		return -1;
	}
	
	return 0;
}

int CSocket::connect(const string& host, const int port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(host.c_str());

	int rst = ::connect(fd_ , (sockaddr*)&addr, sizeof(addr));
	if(0 != rst)
	{
		cout << "connect to server[" << host << ":" << port << "] failed, error = "
				<< WSAGetLastError() <<endl;
		return -1;
	}

	return 0;
}

int CSocket::send_buf(const char* buf,  int& buf_len)
{
	int send_bytes = 0;
	int rest_len = buf_len;
	while( 0 != rest_len)
	{
		int send_len = ::send(fd_, buf + send_bytes, buf_len, 0);
		if(send_len < 0 )
		{
			int err = WSAGetLastError();
			if( EAGAIN == err || EWOULDBLOCK == err)
			{
				buf_len = send_bytes;
				return 1;		//would block
			}
			else if(EINTR == err)
			{
				continue;
			}
			else
			{
				return err;
			}
		}
		send_bytes += send_len;
	}

	return send_bytes;
}