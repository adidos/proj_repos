#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_

class SocketBase;

class TcpSocket : public SocketBase
{
public:
	TcpSocket();
	
	int init();
	
	int send_buf();
	int recv_buf();
	



};

#endif /*TCP_SOCKET_H_*/