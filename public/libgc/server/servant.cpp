
/* ======================================================
* 
* file:		servant.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-28 10:54:37
* 
* ======================================================*/

#include "servant.h"

#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "common/utility.h"
#include "common/configure.h"
#include "common/logger.h"
#include "common/index.h"

#include "event.h"
#include "application.h"
#include "epoll_server.h"
#include "session_manager.h"

#include <iostream>

using namespace std;

extern Configure* g_pConfig;

Servant::Servant(SessionManager* pSessMgr)
	:_acceptor(-1), _epoll_svr_ptr(NULL),_session_mgr_ptr(pSessMgr)
{
}

Servant::~Servant()
{
	close(_acceptor);
	if(NULL != _epoll_svr_ptr)
	{
		delete _epoll_svr_ptr;
		_epoll_svr_ptr = NULL;
	}
}

/**
* brief:
*
* @returns   
*/
int	 Servant::init()
{
	int max_connection = g_pConfig->getInt("max.connection", 65535);

	_epoll_svr_ptr = new EpollServer();
	_epoll_svr_ptr->init(max_connection);

	initAcceptor();
	return 0;
}

/**
* brief:
*
* @returns   
*/
int  Servant::startService()
{
	_epoll_svr_ptr->start();
	this->start();	
	return 0;
}

int Servant::waitForStop()
{
	_epoll_svr_ptr->waitForStop();
	this->waitForStop();
	return 0;
}

/**
* brief:
*/
void Servant::doIt()
{
	sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	while(!_terminate)
	{
		int client = accept(_acceptor, (sockaddr*)&addr, &addr_len);
		if(-1 == client)
		{
			if(EINTR == errno)
				continue;

			LOG4CPLUS_ERROR(FLogger, "accept error: " << strerror(errno));
			_exit(-1);
		}

		LOG4CPLUS_DEBUG(FLogger, "receive a connection from client " 
			<< inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port));
		
		newConnection(client);
	}
}


void Servant::stop()
{
	_epoll_svr_ptr->stop();
	
	_terminate = true;
}

/**
* brief:
*
* @returns   
*/
int Servant::initAcceptor()
{
	int port = g_pConfig->getInt("servant.port");
	if(port < 0 || port >65535)
	{
		cerr << "wrong bind port !" <<endl;
		_exit(-1);
	}

	_acceptor = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == _acceptor)
	{
		perror("socket");
		_exit(-1);
	}
	
	int iReuseAddrFlag=1;

	setsockopt(_acceptor, SOL_SOCKET, SO_REUSEADDR, (char*)&iReuseAddrFlag, sizeof(iReuseAddrFlag));

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	int ret = ::bind(_acceptor, (sockaddr*)&addr, sizeof(addr));
	if(-1 == ret)
	{
		perror("bind");
		_exit(-1);
	}

	ret = listen(_acceptor, 10);
	if(-1 == ret)
	{
		perror("listen");
		_exit(-1);
	}

	cout << "server start on " << port << endl;
	return 0;
}

/**
* brief:
*
* @param client
*
* @returns   
*/
int Servant::newConnection(int client)
{
	setNoBlock(client);

	int seqno = Index::get();
	SessionBasePtr pSession(new SessionBase(client, seqno));

	LOG4CPLUS_DEBUG(FLogger, "create a new session(" << client << "," << seqno << ").");

	bool bret = _session_mgr_ptr->addSession(pSession);

	if(!bret)
	{
		LOG4CPLUS_ERROR(FLogger, "add session to manager failed!");

		return -1;
	}

	//添加session成功后，在创建socket和seqno的关联
	int old_seq =_session_mgr_ptr->addFd(client, seqno);
	if(0 != old_seq)
	{
		LOG4CPLUS_DEBUG(FLogger, "socket " << client <<" was used by " << old_seq
				<< " , find the old session free it!");

		//既然fd已经复用，那么久不能关闭，在session的析构中
		//也不可以close,所以对于每个session直接调用close，不能依赖析构函数
		_session_mgr_ptr->delSession(seqno);
	}

	uint64_t data = U64(seqno, client);

	_epoll_svr_ptr->notify(client, data, EVENT_NEW);

	return 0;
}

