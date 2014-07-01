#ifndef TRANSCEIVER_HANDLE_H
#define TRANSCEIVER_HANDLE_H

#include <map>

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
		AdapterProxyPtr adapter;

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

	int regProxy(int fd, AdapterProxyPtr adapter, TransceiverPtr trans)
	{
		ProxyInfo info;
		info.fd = fd;
		info.adapter = adapter;
		info.trans = trans;

		_proxys[fd] = info;

		return 0;
	}

private:
	typedef map<int, ProxyInfo>::iterator Iterator;
	map<int, ProxyInfo> _proxys;

	CMutex _mutex;

};

#endif
