#ifndef TRANSCEIVER_HANDLE_H
#define TRANSCEIVER_HANDLE_H

#include <map>

#include "common/thread_sync.h"

using namespace std;

class AdapterProxy;
class Transceiver;

class TransceiverHandle
{
public:
	enum{R=0x01, W=0x02};

	struct ProxyInfo
	{
		int event;
		int fd;
		Transceiver* trans;
		AdapterProxy* adapter;

		ProxyInfo():event( -1), fd(-1),
			trans(NULL), adapter(NULL){};
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

	int regProxy(int fd, AdapterProxy* proxy, Transceiver* trans)
	{
		ProxyInfo info;
		info.fd = fd;
		info.adapter = proxy;
		info.trans = trans;

		_proxys[fd] = info;
	}

private:
	typedef map<int, ProxyInfo>::iterator Iterator;
	map<int, ProxyInfo> _proxys;

	CMutex _mutex;

};

#endif
