#ifndef TRANSCEIVER_HANDLE_H
#define TRANSCEIVER_HANDLE_H

#include <map>

#include "common/logger.h"
#include "common/thread_sync.h"

#include "adapter_proxy.h"
#include "transceiver.h"

using namespace std;

class TransceiverHandle
{
public:
	enum{R=0x01, W=0x02};

	struct ProxyInfo
	{
		int event;
		int fd;
		TransceiverPtr trans;
		AdapterProxy* adapter;

		ProxyInfo():event( -1), fd(-1),
			trans(), adapter(){};
		
	};

public:
	
	TransceiverHandle();
	
	~TransceiverHandle();

	int setFDReactor();

public:
	
	void handle(int fd, int evs);

	int handleInput(ProxyInfo& proxy);

	int handleOutput(ProxyInfo& proxy);

	int handleExcept(int fd);

	int regProxy(int fd, AdapterProxy* adapter, TransceiverPtr trans)
	{
		CScopeGuard guard(_mutex);
		ProxyInfo info;
		info.fd = fd;
		info.adapter = adapter;
		info.trans = trans;

		LOG4CPLUS_WARN(FLogger, "adapter ptr is " << adapter);

		_proxys[fd] = info;

		return 0;
	}

private:
	typedef map<int, ProxyInfo>::iterator Iterator;
	map<int, ProxyInfo> _proxys;

	CMutex _mutex;

};

#endif
