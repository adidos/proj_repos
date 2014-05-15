
/* ======================================================
* 
* file:		accept_server.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-15 19:18:27
* 
* ======================================================*/

#include "accept_server.h"

#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

AcceptServer::AcceptServer(EpollServer* pEpollSvr)
	:fd_(-1) /*epoll_svr_ptr_(pEpollSvr)*/
{
}

void AcceptServer::init(short port)
{
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == fd_)
	{
		perror("socket");
		_exit(-1);
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	int ret = bind(fd_, (sockaddr*)&addr, sizeof(addr));
	if(-1 == ret)
	{
		perror("bind");
		_exit(-1);
	}

	ret = listen(fd_, 10);
	if(-1 == ret)
	{
		perror("listen");
		_exit(-1);
	}
}

void AcceptServer::doIt()
{
	sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	while(true)
	{
		int client = accept(fd_, (sockaddr*)&addr, &addr_len);
		if(-1 == client)
		{
			if(EINTR == errno)
				continue;

			perror("accept");
			_exit(-1);
		}
		
		//TODO modify file descriptor attribute

		//epoll_svr_ptr_->notify(client);
	}
}
