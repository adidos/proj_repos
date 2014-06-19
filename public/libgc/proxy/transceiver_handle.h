#ifndef TRANSCEIVER_HANDLE_H
#define TRANSCEIVER_HANDLE_H

#include <map>

using namespace std;

class AdapterProxy;
class CMutex;



class TransceiverHandle
{
public:
	enum{R=0x01, W=0x02};

	struct ProxyInfo
	{
		int evetn;
		int fd;
		Transceiver* trans;
		AdapterProxy* adapter;

		ProxyInfo():event( -1), fd(-1),
			trans(NULL), adap(NULL){};
	};

public:
	
	TransceiverHandle();
	
	~TransceiverHandle();

	setFDReactor();

public:
	
	void handle(int fd, int evs);

	int handleInput(ProxyInfo& proxy);

	int handleOutput(ProxyInfo& proxy);

	int handleExcept(int fd);

	int regProxy(int fd, AdapterProxy* proxy, Transceiver* trans)
	{
		ProxyInfo info;
		info.fd = fd;
		info.adap = proxy;
		info.trans = trans;

		_proxys[fd] = info;
	}

private:
	typedef map<int, ProxyInfo>::iterator Iterator;
	map<int, ProxyInfo> _proxys;

	CMutex _mutex;

};

#endif
