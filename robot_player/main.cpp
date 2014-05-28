#include <iostream>
#include <cstring>
#include <cerrno>

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wd;
	WSAStartup(MAKEWORD(2,2), &wd);

	SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd <0)
	{
		cout << WSAGetLastError() <<endl;
		system("pause");
		exit(-1);
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8910);
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.5.172");
	int rst = connect(fd, (sockaddr*)&addr, sizeof(addr));
	if(rst != 0)
	{
		cout <<WSAGetLastError() <<endl;
		cout << strerror(errno) << endl;
		system("pause");
		exit(-1);
	}

	system("pause");
}