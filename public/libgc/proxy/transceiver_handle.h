#ifndef TRANSCEIVER_HANDLE_H
#define TRANSCEIVER_HANDLE_H

#include <map>

using namespace std;

class AdapterProxy;
class CMutex;

class TransceiverHandle
{
public:
	TransceiverHandle();
	~TransceiverHandle();

	enum{R=0x01, W=0x02};

	setFDReactor();


public:
	void handle(int fd, int evs);

	int handleInput(AdapterProxy* ap);

	int handleOutput(AdapterProxy* ap);

	int handleExcept(AdapterProxy* ap);

	int regProxy(int fd, AdapterProxy* proxy)
	{
		_proxys[fd] = proxy;
	}

private:
	typedef map<int, AdapterProxy*>::iterator Iterator;
	map<int, AdapterProxy*> _proxys;

	CMutex _mutex;

};

#endif
