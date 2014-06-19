#ifndef ADAPTER_PROXY_H
#define ADAPTER_PROXY_H

#include <map>
#include <string>

#include "common/queue.h"

using namespace std;

class ReqMessage;
class FDReactor;
class Transceiver;
class TransceiverHandle;
class DataXCmd;

class AdapterProxy
{
public:
	AdapterProxy(FDReactor* actor, TransceiverHandle* handle);
	~AdapterProxy();

	int initialize(const string& host, short port);

public:
	int invoke(ReqMessage* req);	

	int sendRequest();

	//conncet采用的是非阻塞
	int finishConnect();

	//TODO this interface is not suitable
	int finished(DataXCmd* pCmd);

private:
	string _host;
	short _port;

	FDReactor* _reactor;

	TransceiverHandle* _handle;
	
	Transceiver* _tran;

	map<int, ReqMessage*> _timeout_que;	//所有的请求记录，消息回复后删除

	CMutex _mutex;

	Queue<ReqMessage*> _reqs;		//发送消息队列

	int _conn_timeout_ms;
	
};

#endif 
