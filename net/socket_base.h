#ifndef SOCKET_BASE_H_
#define SOCKET_BASE_H_

#define INVALID_SOCKET -1

class SocketBase
{
public:
	SocketBase():fd_(INVALID_SOCKET)
	{
	}
	
	int set_fd(SOCKET fd)
	{
		if(INVALID_SOCKE == fd)
			return -1;
		
		fd_ = fd;
		return 0;
	}
	
	int get_fd(){return fd_;};
	
	int set_noblock(bool bNoBlock);
	int set_nodelay();
	int set_KeepAlive(int interval_sec);
	int set_send_buf(int buf_size);
	int set_recv_buf(int buf_size);

private:
	SOCKET fd_;

};
#endif /*SOCKET_BASE_H_*/