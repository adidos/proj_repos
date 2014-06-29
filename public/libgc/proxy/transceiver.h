#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <stdint.h>

#include <list>
#include <string>
#include <memory>

#include "common/DataXCmd.h"

using namespace std;

class Transceiver
{
public:
    
    Transceiver(int fd, bool bConnect);

    virtual ~Transceiver();

   	virtual int fd() const;

	bool isValid() const;

public:
	virtual int doConnect(const string& host, short port) = 0;
	
	virtual int doRequest();

	virtual int doResponse(list<DataXCmdPtr>& resps) = 0;

    void writeToSendBuffer(const std::string& msg);

    void close();

public:
	
	bool hasConnected() { return _connected; }

	void setConnected(bool connected) { _connected = connected; }

public:
    
    virtual int send(const void* buf, uint32_t len, uint32_t flag) = 0;

    virtual int recv(void* buf, uint32_t len, uint32_t flag) = 0;

protected:
    int _fd;

	bool _connected;	//表示是否已经连接到服务端

    std::string _send_buffer;

    std::string _recv_buffer;

};

class TcpTransceiver : public Transceiver
{
public:
	TcpTransceiver(int fd, bool bConnect);
	~TcpTransceiver(){};

public:
	virtual int doConnect(const string& host, short port);
	
	virtual int doResponse(list<DataXCmdPtr>& resps);

	virtual int send(const void* buf, uint32_t len, uint32_t flag);

    virtual int recv(void* buf, uint32_t len, uint32_t flag);

	int reset();
};

typedef std::shared_ptr<Transceiver> TransceiverPtr;

#endif
