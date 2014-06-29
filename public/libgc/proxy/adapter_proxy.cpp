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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>


AdapterProxy::AdapterProxy(FDReactor* actor, TransceiverHandle* handle)
	:_host(""), _port(0), _reactor(actor), _handle(handle),_reqs(10240)
{
	_trans_num = 3; // 默认每个adapter proxy 3个连接
}

AdapterProxy::~AdapterProxy()
{
	if(NULL != _tran)
	{
		delete _tran;
		_tran = NULL;
	}
}

/**
* brief:
*
* @param host
* @param port
*
* @returns   
*/
int AdapterProxy::initialize(const string& host, short port)
{
	_host = host;
	_port = port;

	for(int i = 0; i < _trans_num; i++)
	{
		

	}
	_tran = new TcpTransceiver();

		
	int ret = _tran->doConnect(host, port);
	
	if(0 == ret)
	{
		_reactor->regHandle(_tran->fd(), EPOLLIN|EPOLLOUT, _handle);

		_handle->regProxy(_tran->fd(), this, _tran);
	}
	
	return 0;
}

/**
* brief:
*
* @returns   
*/
int AdapterProxy::sendRequest()
{
	if(_reqs.empty())
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "send queue is empty!");
		return 0;
	}

	//还未成功连接
	if(!_tran->hasConnected())
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "transceiver is not connected!");

		return 0;
	}

	int length = 0;
	
	ReqMessagePtr ptr;
	
	while(_reqs.pop(ptr, 0))
	{
		if(ptr == NULL) return 0;

		//TODO make a abstract interface for the protocol encode
		
		DataXCmdPtr pCmd = ptr->req;

		if(NULL == pCmd)
		{
			LOG4CPLUS_ERROR(CLogger::logger, "the request command is NULL!");
		}

		int len = pCmd->header_length() + pCmd->body_length();

		string buffer(len , '\0');

		pCmd->encode((byte*)buffer.c_str(), len);		
	
		_tran->writeToSendBuffer(buffer);	

		length += buffer.length();
	}

	LOG4CPLUS_DEBUG(CLogger::logger, "write " << length<< " bytes to send buffer!");

	return length;
}

/**
* brief:
*
* @param req
*
* @returns   
*/
int AdapterProxy::invoke(ReqMessagePtr req)
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

/**
* brief:
*
* @returns   
*/
int AdapterProxy::finishConnect()
{
	if(_tran->hasConnected())
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "transceiver is connect already!");
		return 0;
	}

	int val = 0;
    socklen_t len = static_cast<socklen_t>(sizeof(int));
    if (::getsockopt(_tran->fd(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&val), &len) == -1 || val)
    {
		LOG4CPLUS_ERROR(CLogger::logger, "connect to server failed!");
        //连接失败，从事件反应器中删除该事件
        _reactor->unregHandle(_tran->fd(), EPOLLIN | EPOLLOUT, _handle);

        _tran->close();

        return -1;
    }

   	LOG4CPLUS_DEBUG(CLogger::logger, "Adapter conncet to Server " << _host << ":" << _port << " Success!");
    _tran->setConnected(true);

	return 0;
}

/**
* brief:
*
* @param pCmd
*
* @returns   
*/
int AdapterProxy::finished(DataXCmdPtr pCmd)
{
	if(!pCmd)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "DataXCmdPtr is invalid!");
		return -1;
	}

	int req_id = pCmd->get_release_id();
	string name = pCmd->get_cmd_name();

	LOG4CPLUS_DEBUG(CLogger::logger, "receive response[" << name << "] "
			<< ", request id is " << req_id);

	CScopeGuard guard(_mutex);
	map<int, ReqMessagePtr>::iterator iter = _timeout_que.find(req_id);

	if(iter == _timeout_que.end())
	{
		LOG4CPLUS_WARN(CLogger::logger, "can't find request " << req_id);

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

