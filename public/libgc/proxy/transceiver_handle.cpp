#include "transceiver_handle.h"


TransceiverHandle::TransceiverHandle()
{

}

TransceiverHandle::~TransceiverHandle()
{

}

void TransceiverHandle::handle(int fd, int evs)
{
	ProxyInfo proxy;
	{
		CScopeGuard guard(_mutex);
		Iteartor iter = _proxys.find(fd);

		if(iter != _proxys.end())
		{
			proxy = iter->second;
		}
	}
	
	if(evs &  TransceiverHandle::W)
	{
		handleOutput(proxy);
	}

	if(evs & TransceiverHandle::R)
	{
		handleInput(proxy);
	}
}

int TransceiverHandle::handleOutput(ProxyInfo& proxy)
{
	if(NULL == proxy.adapter || NULL == proxy.trans)
		reutrn -1;

	while(proxy.adapter->sendRequest() > 0 && proxy.trans->doRequest() >= 0)

	return 0;
}

int TransceiverHandle::handleInput(ProxyInfo& proxy)
{
	if(NULL == proxy.adapter || NULL == proxy.trans)
	{
		reutrn -1;
	}

	proxy.adapter->finishConnect();
		
	list<DataXCmd*> resps;

	if(proxy.trans->doResponse(resps) > 0)
	{
		list<DataXCmd*>::iterator iter =resps.begin();
		for( ; iter != resps.end(); ++iter)
		{
			proxy.adapter->finished(*iter);
		}		
	}

	return 0;
}