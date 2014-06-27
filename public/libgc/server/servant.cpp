
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

	while(true)
	{
		int client = accept(_acceptor, (sockaddr*)&addr, &addr_len);
		if(-1 == client)
		{
			if(EINTR == errno)
				continue;

			LOG4CPLUS_ERROR(CLogger::logger, "accept error: " << strerror(errno));
			_exit(-1);
		}

		LOG4CPLUS_DEBUG(CLogger::logger, "receive a connection from client " 
			<< inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port));
		
		newConnection(client);
	}
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
	int seqno =	_session_mgr_ptr->getSeqno8Fd(client);
	if(-1 != seqno) //fd被重复使用了，说明前一个连接已经断开，需要释放掉session
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "socket " << client <<" reused, find the old session free it!");
		SessionBase* ptr = _session_mgr_ptr->getSession(seqno);
		_session_mgr_ptr->freeSession(ptr);
	}

	SessionBase* pSession = _session_mgr_ptr->getIdleSession();
	if(NULL == pSession)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "get idle session failed!");
		return -1;
	}

	setNoBlock(client);
	pSession->setFd(client);

	_session_mgr_ptr->addFd(client, seqno);
	bool bret = _session_mgr_ptr->addSession(pSession);
	if(!bret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "add session to manager failed!");

		_session_mgr_ptr->freeSession(pSession);
		return -1;
	}

	seqno = pSession->getSeqno();
	uint64_t data = U64(seqno, client);

	_epoll_svr_ptr->notify(client, data, EVENT_NEW);

	return 0;
}
