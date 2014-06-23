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

#include "common/utility.h"
#include "server/epoll.h"
#include "common/DataXCmd.h"
#include "common/logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>


AdapterProxy::AdapterProxy(FDReactor* actor, TransceiverHandle* handle)
	:_host(""), _port(0), _reactor(actor), _handle(handle),_reqs(10240)
{

}

AdapterProxy::~AdapterProxy()
{
	if(NULL != _tran)
	{
		delete _tran;
		_tran = NULL;
	}
}

int AdapterProxy::initialize(const string& host, short port)
{
	_host = host;
	_port = port;

	_tran = new TcpTransceiver();
	int ret = _tran->doConnect(host, port);

	if(ret == 0)
	{
		_reactor->regHandle(_tran->fd(), EPOLLIN|EPOLLOUT, _handle);

		_handle->regProxy(_tran->fd(), this, _tran);
	}

	return 0;
}

int AdapterProxy::sendRequest()
{
	if(_reqs.empty()) return 0;

	int len = 0;
	
	ReqMessage* ptr = NULL;
	
	while(_reqs.pop(ptr))
	{
		if(ptr == NULL) return 0;

		//TODO make a abstract interface for the protocol encode
		
		DataXCmd* pCmd = ptr->req;

		int len = pCmd->header_length() + pCmd->body_length();

		string buffer(len , '\0');

		pCmd->encode((byte*)buffer.c_str(), len);		
	
		_tran->writeToSendBuffer(buffer);	

		len += buffer.length();
	}	

	return len;
}

int AdapterProxy::invoke(ReqMessage * req)
{
	//将消息加入到队列
	if(! _reqs.push(req))
	{
		LOG4CPLUS_ERROR(CLogger::logger, "push request to queue failed!");
		return -1;
	}

	{
		CScopeGuard guard(_mutex);
		_timeout_que[req->id] = req;
	}

	LOG4CPLUS_DEBUG(CLogger::logger, "push request[" << req->id << "] to queue");
	_reactor->notify(_tran->fd());

	return 0;
}

int AdapterProxy::finishConnect()
{
	if(_tran->hasConnected()) return 0;

	int val = 0;
    socklen_t len = static_cast<socklen_t>(sizeof(int));
    if (::getsockopt(_tran->fd(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&val), &len) == -1 || val)
    {
        //连接失败，从事件反应器中删除该事件
        _reactor->unregHandle(_tran->fd(), EPOLLIN | EPOLLOUT, _handle);

        _tran->close();

        return -1;
    }

   	LOG4CPLUS_DEBUG(CLogger::logger, "Adapter conncet to Server " << _host << ":" << _port << " Success!");
    _tran->setConnected(true);

	return 0;
}

int AdapterProxy::finished(DataXCmd* pCmd)
{
	if(NULL == pCmd)
		return -1;


	int req_id = pCmd->get_release_id();

	CScopeGuard guard(_mutex);
	map<int, ReqMessage*>::iterator iter = _timeout_que.find(req_id);

	if(iter == _timeout_que.end())
	{
		LOG4CPLUS_WARN(CLogger::logger, "can't find request " << req_id);
		delete pCmd;
		pCmd = NULL;
	}

	ReqMessage* req = iter->second;

	_timeout_que.erase(iter);
	
	//maybe the request is timeout,so released
	if(NULL == req) return 0;
	
	req->resp = pCmd;

	if(req->type == SYNC_CALL)
	{
		return req->proxy->finished(req);
	}
	
	return 0;	
}

