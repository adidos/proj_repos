
/* ======================================================
* 
* file:		proxy_group.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-20 10:42:28
* 
* ======================================================*/

#ifndef PROXY_CLIENT_H
#define PROXY_CLIENT_H

#include <map>
#include <string>
#include <memory>

using namespace std;


class FDReactor;
class ServantProxy;
class AdapterProxy;
class TransceiverHandle;

class ProxyClient
{
public:
	
	/**
	* constructor
	*/
	ProxyClient();


	/**
	* descontructor
	*/
	~ProxyClient();

public:

	/**
	* brief:
	*
	* @returns   
	*/
	static ProxyClient* Instance()
	{
		if(NULL == _instance)
		{
			_instance = new ProxyClient();
		}

		return _instance;
	}
	
	/**
	* brief: get servat and adapter info from 
	*		configure file and create them 
	*
	* @returns   
	*/
	int	initialize();
	
	/**
	* brief: get servant proxy by name
	*
	* @param name
	*
	* @returns   
	*/
	ServantProxy* getServantPrxy(const string& name);

private:

	FDReactor* _reactor;

	TransceiverHandle* _trans_handle;

	AdapterProxy* _adapter;

	typedef map<string, ServantProxy*>::iterator Iterator;

	map<string, ServantProxy*> _array;

	static ProxyClient* _instance;
};

#endif //PROXY_CLIENT_H
