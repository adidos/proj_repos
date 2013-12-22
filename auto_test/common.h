#ifndef APP_COMMON_H_
#define APP_COMMON_H_

#include <iostream>

#include <WinSock2.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int init_net()
{
	WSADATA wsaData;
	int rst = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(rst != 0)
	{
		cout <<"error : " << WSAGetLastError() <<endl;
		exit(-1);
	}

	return 0;
}

int fini_net()
{
	WSACleanup();
}

#endif /*APP_COMMON_H_*/
