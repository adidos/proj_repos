#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_

class SocketBase;

class TcpSocket : public SocketBase
{
public:
	TcpSocket();
	
	int init();
	
	int bind();
	int listen();
	
	int connect();
	
	int send_buf();
	int recv_buf();
	
	int close();
};

#endif /*TCP_SOCKET_H_*/