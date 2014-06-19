#include "adapter_proxy.h"

#include "message.h"
#include "fd_reactor.h"
#include "transceiver_handle.h"
#include "transceiver.h"

#include "common/utility.h"
#include "common/epoll.h"



AdapterProxy::AdapterProxy(FDReactor* actor, TransceiverHandle* handle)
	:_host(""), _port(port), _reactor(actor), _handle(handle)
{

}

AdapterProxy::~AdapterProxy()
{
	if(NULL != _handle)
	{
		delete _handle;
		_handle = NULL;
	}
}

int AdapterProxy::initialize(const string& host, short port)
{
	_host = host;
	_port = port;

	_tran = new Transceiver();
	int ret = _tran->doConnect(host, port);

	if(ret == 0)
	{
		_reactor->regHandle(_tran->fd(), EPOLLIN|EPOLLOUT, _handle);

		_handle->regProxy(_tran->fd(), this, _tran);
	}	
}

int AdapterProxy::sendRequest()
{
	if(_reqs.empty()) return 0;

	int len = 0;
	
	ReqMessage* ptr = NULL
	
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
	//����Ϣ���뵽����
	if(! _reqs->push(req, 10))
	{
		LOG4CPLUS_ERROR(CLogger::logger, "push request to queue failed!");
		return -1;
	}

	{
		CScopeGuard guard(_mutex);
		_timeout_que[req->req_id] = req;
	}

	LOG4CPLUS_DEBUG(CLogger::logger, "push request[" << req->req_id << "] to queue");
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
        //����ʧ�ܣ����¼���Ӧ����ɾ�����¼�
        _reactor->unregHandle(_tran->fd(), EPOLLIN | EPOLLOUT, _handle);

        _tran->close();

        return ;
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

