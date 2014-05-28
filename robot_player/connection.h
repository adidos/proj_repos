
/* ======================================================
* 
* file:		connection.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-24 15:18:06
* 
* ======================================================*/
#ifndef DOS_CONNECTION_H 
#define DOS_CONNECTION_H

#include <string>

using namespace std;

class Connection
{
public:
	Connection();

	int init();

	int connect(const string& host, const int port);

	int send_buf(const char* buf,  int& buf_len);
	 
	int recv_buf(char* buf, int& buf_len);

	int disconnect();

private:
	int _fd;

	string _recv_buff;
	string _send_buff;
};
#endif //DOS_CONNECTION_H
