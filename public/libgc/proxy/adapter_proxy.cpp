/* ======================================================
* 
* file:		adapter_proxy.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-19 23:13:46
* 
* ======================================================*/

#include "adapter_proxy.h"

#include "message.h"
#include "fd_reactor.h"
#include "transceiver_handle.h"
#include "transceiver.h"
#include "servant_proxy.h"

#include "common/utility.h"
#include "server/epoll.h"
#include "common/DataXCmd.h"
#include "common/logger.h"
#include "common/function_trace.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


AdapterProxy::AdapterProxy(FDReactor* actor, TransceiverHandle* handle)
	:_host(""), _port(0), _reactor(actor), _handle(handle),_reqs(10240)
{

}

AdapterProxy::~AdapterProxy()
{
}

int AdapterProxy::initialize(const string& host, short port)
{
	_host = host;
	_port = port;

<<<<<<< HEAD
	_tran = new TcpTransceiver();

		
	int ret = _tran->doConnect(host, port);
	
	if(0 == ret)
	{
		_reactor->regHandle(_tran->fd(), EPOLLIN|EPOLLOUT, _handle);

		_handle->regProxy(_tran->fd(), this, _tran);
=======
	//when init, create three connection
	for(size_t i = 0; i < _trans_num; i++)
	{
		TransceiverPtr trans = doReconnect();
		
		if(! trans)
		{
			LOG4CPLUS_WARN(FLogger, "proxy connect to servant failed!");
		}
>>>>>>> multi
	}
	
	return 0;
}

<<<<<<< HEAD
int AdapterProxy::sendRequest()
=======
/**
* brief:
*
* @returns   
*/
int AdapterProxy::sendRequest(TransceiverPtr& trans)
>>>>>>> multi
{
	int length = 0;
	
	ReqMessagePtr ptr;
	
	while(_reqs.pop(ptr, 0))
	{
		if(! ptr) return 0;

		//TODO make a abstract interface for the protocol encode
		
		DataXCmdPtr pCmd = ptr->req;

		if(! pCmd)
		{
			LOG4CPLUS_ERROR(FLogger, "the request command is NULL!");
			continue;
		}

		int len = pCmd->header_length() + pCmd->body_length();

		string buffer(len , '\0');

		pCmd->encode((byte*)buffer.c_str(), len);		
	
		trans->writeToSendBuffer(buffer);	

		length += buffer.length();
	}

	LOG4CPLUS_DEBUG(FLogger, "write " << length<< " bytes to send buffer!");

	return length;
}

int AdapterProxy::invoke(ReqMessagePtr req)
{
	//将消息加入到队列
	if(! _reqs.push(req))
	{
		LOG4CPLUS_ERROR(FLogger, "push request to queue failed!");
		return -1;
	}

	TransceiverPtr trans = selectTransceiver();

	//Transceiver is NULL or invalid return fail
	if(! trans || ! trans->isValid())
		return -1;

	_reactor->notify(trans->fd());

	{
		CScopeGuard guard(_mutex);
		_timeout_que[req->id] = req;
	}

	LOG4CPLUS_DEBUG(FLogger, "push request[" << req->id << "] to queue");

	return 0;
}

<<<<<<< HEAD
int AdapterProxy::finishConnect()
=======
/**
* brief:
*
* @returns   
*/
TransceiverPtr AdapterProxy::doReconnect()
>>>>>>> multi
{
	//防止servant未启动,不断地增加连接
	if(_trans.size() == _trans_num) return TransceiverPtr();

	TransceiverPtr trans;

	int fd = ::socket(AF_INET, SOCK_STREAM, 0);
	
	if(fd < 0)
	{
		LOG4CPLUS_ERROR(FLogger, "socket error:" << strerror(errno));

		return trans;
	}

	setNoBlock(fd);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = inet_addr(_host.c_str());

	int ret = ::connect(fd, (sockaddr*)&addr, (socklen_t)sizeof(addr));

	int err_no = errno;

	LOG4CPLUS_DEBUG(FLogger, "connect to server " << _host 
				<< ":" << _port << " error: " << strerror(err_no));

	bool isconnect = false;
	if(0 == ret)
	{
		isconnect = true;
	}
	else if(ret < 0 && EINPROGRESS != err_no)
	{
		LOG4CPLUS_ERROR(FLogger, "connect error:" << strerror(err_no));
		::close(fd);
		return trans;
	}

	TcpTransceiver* ptr = new TcpTransceiver(fd, isconnect);

	trans.reset(ptr);

	_reactor->regHandle(fd , EPOLLIN|EPOLLOUT, _handle);

	_handle->regProxy(fd, AdapterProxyPtr(this), trans);

	{
		CScopeGuard guard(_trans_mutex);
		_trans.push_back(trans);
	}
	
	return trans;
}

/**
* brief:
*
* @returns   
*/
int AdapterProxy::finishConnect(TransceiverPtr& trans)
{
	if(trans->hasConnected())
	{
		LOG4CPLUS_DEBUG(FLogger, "transceiver is connect already!");
		return 0;
	}

	int val = 0;
    socklen_t len = static_cast<socklen_t>(sizeof(int));

    if (::getsockopt(trans->fd(), SOL_SOCKET, SO_ERROR, 
		reinterpret_cast<char*>(&val), &len) == -1 || val)
    {
		LOG4CPLUS_ERROR(FLogger, "connect to server failed!");
        //连接失败，从事件反应器中删除该事件
        _reactor->unregHandle(trans->fd(), EPOLLIN | EPOLLOUT, _handle);

        trans->close();

		//where transceiver close ,need to refresh transceivers
		refreshTransceiver();

        return -1;
    }

   	LOG4CPLUS_DEBUG(FLogger, "Adapter conncet to Server " << _host 
				<< ":" << _port << " Success!");

    trans->setConnected(true);

	_reactor->notify(trans->fd());	

	return 0;
}

<<<<<<< HEAD
=======
/**
* brief: select a valid transceiver to send msg
*
* @returns   
*/
TransceiverPtr AdapterProxy::selectTransceiver()
{
	CScopeGuard guard(_trans_mutex);

	typedef vector<TransceiverPtr>::iterator Iterator;

	while(! _trans.empty())
	{
		int index = rand() % _trans.size();

		Iterator iter = _trans.begin() + index;
	
		TransceiverPtr ptr = *iter;

		if(ptr && ptr->isValid()) return ptr;

		iter = _trans.erase(iter);
	}

	//如果没有有效的,重新连接建立
	return doReconnect();
}

/**
* brief: delete invalid transceiver
*/
void AdapterProxy::refreshTransceiver()
{
	CScopeGuard guard(_trans_mutex);

	vector<TransceiverPtr>::iterator iter = _trans.begin();

	while(iter != _trans.end())
	{
		if((*iter)->isValid())
			++iter;
		else //no problem ?
			iter = _trans.erase(iter);
	}
}

/**
* brief:
*
* @param pCmd
*
* @returns   
*/
>>>>>>> multi
int AdapterProxy::finished(DataXCmdPtr pCmd)
{
	if(!pCmd)
	{
		LOG4CPLUS_ERROR(FLogger, "DataXCmdPtr is invalid!");
		return -1;
	}

	int req_id = pCmd->get_release_id();
	string name = pCmd->get_cmd_name();

	LOG4CPLUS_DEBUG(FLogger, "receive response[" << name << "] "
			<< ", request id is " << req_id);

	CScopeGuard guard(_mutex);
	map<int, ReqMessagePtr>::iterator iter = _timeout_que.find(req_id);

	if(iter == _timeout_que.end())
	{
		LOG4CPLUS_WARN(FLogger, "can't find request " << req_id);

		return -1;
	}

	ReqMessagePtr req = iter->second;

	_timeout_que.erase(iter);
	
	req->resp = pCmd;

	if(req->type == SYNC_CALL)
	{
		return req->proxy->finished(req);
	}
	
	return 0;	
}

