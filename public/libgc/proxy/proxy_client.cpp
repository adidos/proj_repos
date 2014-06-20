
/* ======================================================
* 
* file:		proxy_client.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-20 10:56:00
* 
* ======================================================*/

#include "proxy_client.h"

#include "fd_reactor.h"
#include "transceiver_handle.h"

extern Configure* g_pConfig;

ProxyClient::ProxyClient() :_trans_handle(NULL),
	 _reactor(NULL), _adapter(NULL)
{

}

ProxyClient::~ProxyClient
{
	if(NULL != _trans_handle)
	{
		delete _trans_handle;
	}

	if(NULL != _reactor)
	{
		delete _reactor;
	}
}

ProxyClient::initialize()
{
	string host = g_pConfig->getString("user.server.host");
	short port = (short)g_pConfig->getInt("user.server.port");

	string name = g_pConfig->getString("user.servant.name");
	int timeout_ms = g_pConfig->getInt("user.servant.timeout.ms");	

	//
	_reactor = new FDReactor();
	_reactor->start();

	_trans_handle = new TransceiverHandle();

	//
	_adapter = new AdapterProxy(_reactor, _trans_handle);

	_adapter->initialize(host, port);

	//
	ServantProxyPtr ptr(new UserProxy());
	ptr->setTimeout(timeout_ms);
	ptr->setName(name);
	ptr->regAdapterProxy(_adapter);
	
	_array[name] = ptr;

	_reactor->waitForStop();
}


ServantProxyPtr ProxyClient::getServantPrxy(const string& name)
{
	Iterator iter = _array.find(name);

	if(iter == _array.end())
	{
		LOG4CPLUS_ERROR(CLogger::logger, "could't find " << name
			<< " servant proxy!");	
		
		return ServantProxyPtr(NULL);
	}
	
	return iter->second;
}
