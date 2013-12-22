#ifndef DOS_SOCKET_H_
#define DOS_SOCKET_H_

#include <string>

#include <WinSock2.h>

using namespace std;

class CSocket
{
public:
	CSocket();

	int init();

	int connect(const string& host, const int port);

	int send_buf(const char* buf,  int& buf_len);
	 
	int recv_buf(char* buf, int& buf_len);

	int disconnect();

private:
	SOCKET fd_;
};
#endif /*DOS_SOCKET_H_*/