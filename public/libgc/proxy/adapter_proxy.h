#ifndef ADAPTER_PROXY_H
#define ADAPTER_PROXY_H

#include <map>
#include <vector>

#include "common/queue.h"

class AdapterProxy
{
public:
	AdapterProxy(const string& host, port);
	~AdapterProxy();



public:
	int invoke(ReqMessage* req);


	

	

private:
	vector<Transceiver*> _trans;	//TODO multy transceiver per adapterproxy, now only one in vector

	map<int, ReqMessage* req>;

	Queue<ReqMessage> _reqs;
	

}

#endif 
