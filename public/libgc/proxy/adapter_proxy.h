
/* ======================================================
* 
* file:		adapter_proxy.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-19 23:08:40
* 
* ======================================================*/

#ifndef ADAPTER_PROXY_H
#define ADAPTER_PROXY_H

#include <map>
#include <string>

#include "common/queue.h"
#include "common/thread_sync.h"

using namespace std;

class ReqMessage;
class FDReactor;
class Transceiver;
class TransceiverHandle;
class DataXCmd;

class AdapterProxy
{
public:

	/**
	* constructor
	*/
	AdapterProxy(FDReactor* actor, TransceiverHandle* handle);

	/**
	* destructor
	*/
	~AdapterProxy();

	/**
	* brief:
	*
	* @param host
	* @param port
	*
	* @returns   
	*/
	int initialize(const string& host, short port);

public:

	/**
	* brief:
	*
	* @param req
	*
	* @returns   
	*/
	int invoke(ReqMessage* req);	

	/**
	* brief:
	*
	* @returns   
	*/
	int sendRequest();

	/**
	* brief:
	*
	* @returns   
	*/
	int finishConnect();

	
	/**
	* brief:
	* TODO this interface is not suitable
	* @param pCmd
	*
	* @returns   
	*/
	int finished(DataXCmd* pCmd);

private:
	string _host;

	short _port;

	FDReactor* _reactor;

	TransceiverHandle* _handle;
	
	Transceiver* _tran;

	Queue<ReqMessage*> _reqs;		//发送消息队列

	map<int, ReqMessage*> _timeout_que;	//所有的请求记录，消息回复后删除

	CMutex _mutex;

	int _conn_timeout_ms;
};

#endif 
