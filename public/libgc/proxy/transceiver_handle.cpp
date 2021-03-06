#include "transceiver_handle.h"
#include "transceiver.h"
#include "adapter_proxy.h"

#include "common/logger.h"
#include "common/DataXCmd.h"

#include <list>

using namespace std;


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
		Iterator iter = _proxys.find(fd);

		if(iter != _proxys.end())
		{
			proxy = iter->second;
		}
		else
			LOG4CPLUS_ERROR(FLogger, "didn't find fd " << fd << " proxy info");
	}

	if(NULL != proxy.adapter)
	{
		proxy.adapter->finishConnect(proxy.trans);
		
		if(evs & TransceiverHandle::W)
		{
			LOG4CPLUS_DEBUG(FLogger, proxy.fd << " receive a epoll out event!");
			handleOutput(proxy);
		}

		if(evs & TransceiverHandle::R)
		{
			LOG4CPLUS_DEBUG(FLogger, proxy.fd << " receive a epoll in event!");
			handleInput(proxy);
		}
		
		//transceiver无效(socket关闭),
		if(! proxy.trans->isValid())
		{
			proxy.adapter->refreshTransceiver();

			_proxys.erase(fd);
		}
	}
	else
		LOG4CPLUS_ERROR(FLogger, "fd " << fd << " adapter is NULL !");
}


int TransceiverHandle::handleExcept(int fd)
{	
	LOG4CPLUS_ERROR(FLogger, "fd " << fd << " occur a error!");

	ProxyInfo proxy;
	{
		CScopeGuard guard(_mutex);
		Iterator iter = _proxys.find(fd);

		if(iter != _proxys.end())
		{
			proxy = iter->second;
			_proxys.erase(iter);
		}
	}

	if(proxy.trans)
	{
		proxy.trans->close();

		proxy.adapter->refreshTransceiver();
	}

	return 0;
}

int TransceiverHandle::handleOutput(ProxyInfo& proxy)
{
	if(NULL == proxy.adapter || ! proxy.trans)
		return -1;

	while(proxy.adapter->sendRequest(proxy.trans) > 0 && proxy.trans->doRequest() >= 0);

	return 0;
}

int TransceiverHandle::handleInput(ProxyInfo& proxy)
{
	if(NULL == proxy.adapter || ! proxy.trans)
	{
		return -1;
	}
		
	list<DataXCmdPtr> resps;

	if(proxy.trans->doResponse(resps) > 0)
	{
		list<DataXCmdPtr>::iterator iter = resps.begin();
		for( ; iter != resps.end(); ++iter)
		{
			DataXCmdPtr ptr = *iter;

			if(!ptr)
			{
				LOG4CPLUS_ERROR(FLogger, "INVALID DataXCmdPtr! fuck!");
				continue;
			}

			proxy.adapter->finished(*iter);
		}		
	}

	return 0;
}
