
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
#include <vector>
#include <string>

#include "common/DataXCmd.h"
#include "common/queue.h"
#include "common/thread_sync.h"

#include "message.h"
#include "transceiver.h"

using namespace std;

class FDReactor;
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
	int sendRequest(TransceiverPtr& trans);

	/**
	* brief:
	*
	* @returns   
	*/
	TransceiverPtr doReconnect();

	/**
	* brief:
	*
	* @returns   
	*/
	int finishConnect(TransceiverPtr& trans);

	/**
	* brief:
	* TODO this interface is not suitable
	* @param pCmd
	*
	* @returns   
	*/
	int finished(DataXCmdPtr pCmd);

	/**
	* brief:
	*/
	void refreshTransceiver();
	
private:

	/**
	* brief:
	*
	* @returns   
	*/
	TransceiverPtr selectTransceiver();

private:
	string _host;

	short _port;

	FDReactor* _reactor;

	TransceiverHandle* _handle;
	
	//Transceiver* _tran;
	vector<TransceiverPtr> _trans;

	CMutex _trans_mutex;

	size_t _trans_num;

	Queue<ReqMessagePtr> _reqs;		//发送消息队列

	map<int, ReqMessagePtr> _timeout_que;	//所有的请求记录，消息回复后删除

	CMutex _mutex;
};

typedef shared_ptr<AdapterProxy> AdapterProxyPtr;

#endif 
