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

	//conncet���õ��Ƿ�����
	int finishConnect();

	//TODO this interface is not suitable
	int finished(DataXCmd* pCmd);

private:
	string _host;
	short _port;

	FDReactor* _reactor;

	TransceiverHandle* _handle;
	
	Transceiver* _tran;

	map<int, ReqMessage*> _timeout_que;	//���е������¼����Ϣ�ظ���ɾ��

	CMutex _mutex;

	Queue<ReqMessage*> _reqs;		//������Ϣ����

	int _conn_timeout_ms;
	
};

#endif 
