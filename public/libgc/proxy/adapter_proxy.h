
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

#include "common/DataXCmd.h"
#include "common/queue.h"
#include "common/thread_sync.h"

#include "message.h"

using namespace std;

class FDReactor;
class Transceiver;
class TransceiverHandle;

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
	int invoke(ReqMessagePtr req);	

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
	int finished(DataXCmdPtr pCmd);

private:
	string _host;

	short _port;

	FDReactor* _reactor;

	TransceiverHandle* _handle;
	
	Transceiver* _tran;

	Queue<ReqMessagePtr> _reqs;		//发送消息队列

	map<int, ReqMessagePtr> _timeout_que;	//所有的请求记录，消息回复后删除

	CMutex _mutex;

	int _conn_timeout_ms;
};

#endif 
