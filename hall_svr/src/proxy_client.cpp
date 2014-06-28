
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
#include "user_proxy.h"

#include "proxy/fd_reactor.h"
#include "proxy/servant_proxy.h"
#include "proxy/transceiver_handle.h"
#include "proxy/adapter_proxy.h"

#include "common/configure.h"
#include "common/logger.h"

extern Configure* g_pConfig;

ProxyClient* ProxyClient::_instance = NULL;

ProxyClient::ProxyClient() : _reactor(NULL), 
	_trans_handle(NULL),_adapter(NULL)
{

}

ProxyClient::~ProxyClient()
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

int ProxyClient::initialize()
{
	LOG4CPLUS_DEBUG(CLogger::logger, "run here?");
	//
	_reactor = new FDReactor();
	_reactor->start();

	_trans_handle = new TransceiverHandle();

	//
	string host = g_pConfig->getString("user.server.host");
    short port = (short)g_pConfig->getInt("user.server.port");

	_adapter = new AdapterProxy(_reactor, _trans_handle);
	_adapter->initialize(host, port);

	//
    string name = g_pConfig->getString("user.servant.name");
    int timeout_ms = g_pConfig->getInt("user.servant.timeout.ms"); 

	//
	ServantProxy* ptr = new UserProxy(0x10);
	ptr->setTimeout(timeout_ms);
	ptr->setName(name);
	ptr->regAdapterProxy(_adapter);
	
	_array[name] = ptr;

//	_reactor->waitForStop();	

	return 0;
}


ServantProxy* ProxyClient::getServantPrxy(const string& name)
{
	Iterator iter = _array.find(name);

	if(iter == _array.end())
	{
		LOG4CPLUS_ERROR(CLogger::logger, "could't find " << name
			<< " servant proxy!");	
		
		return NULL;
	}
	
	return iter->second;
}
