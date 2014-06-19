#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <stdint.h>

#include <list>
#include <string>

using namespace std;

class DataXCmd;

class Transceiver
{
public:
    
    Transceiver();

    virtual ~Transceiver();

   	virtual int fd() const;

	bool isValid() const;

public:
	virtual int doConnect(const string& host, short port) = 0;
	
	virtual int doRequest();

	virtual int doResponse(list<DataXCmd*>& resps) = 0;

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

    std::string _send_buffer;

    std::string _recv_buffer;

	bool _connected;	//表示是否已经连接到服务端
};

class TcpTransceiver : public Transceiver
{
public:
	TcpTransceiver(){};
	~TcpTransceiver(){};

public:
	virtual int doConnect(const string& host, short port);
	
	virtual int doResponse(list<DataXCmd*>& resps);

	virtual int send(const void* buf, uint32_t len, uint32_t flag);

    virtual int recv(void* buf, uint32_t len, uint32_t flag);
};

#endif
