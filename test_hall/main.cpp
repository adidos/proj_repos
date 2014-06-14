#include <iostream>
#include <vector>
#include <string>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#include "common/logger.h"
#include "common/thread.h"
#include "event_server.h"
#include "command_wrapper.h"

EventServer* pEventSvr = NULL;

class WorkerThread : public CThread
{
public:
	int init(int size);

protected:
	virtual void doIt();

private:
	int Connect();

private:
	vector<int> _array;

};

int WorkerThread::init(int size)
{
		
	for(int i = 0; i < size; ++i)
	{
		int client = Connect();	
		if(client == -1)
			continue;
		
		_array.push_back(client);	
		pEventSvr->addEvent(client, EPOLLIN | EPOLLET, client);
	}
	return 0;
}

int WorkerThread::Connect()
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8900);
	addr.sin_addr.s_addr = inet_addr("10.161.159.33");

	int client = socket(AF_INET, SOCK_STREAM, 0);
	int ret = connect(client, (sockaddr*)&addr, sizeof(addr));
	if(ret != 0)
	{
		perror("connect");
		close(client);
		return -1;
	}
	LOG4CPLUS_DEBUG(CLogger::logger,  client << " connecto to server success!");
	return client;
}

void WorkerThread::doIt()
{
	int index = 0;
	cout << _id << " start work!" <<endl;
	while(true)
	{
		vector<int>::iterator iter = _array.begin();
		for(;iter != _array.end(); ++iter)
		{
			string buffer = createGetDir(*iter, 1);
			send(*iter, buffer.c_str(), buffer.length(), 0);
			
			usleep(5000);
			buffer.clear();
			
			buffer = createGetItem(*iter, 1, 0);
			send(*iter, buffer.c_str(), buffer.length(), 0);
			usleep(5000);
		}

		int begin = index * _array.size() / 10;
		int end = (index + 1) * _array.size() / 10;
		for(int i = begin; i < end; ++i)
		{
			int fd = _array[i];
			close(fd);
			pEventSvr->delEvent(fd, EPOLLIN | EPOLLET, fd);

			fd = Connect();
			_array[i] = fd;
			pEventSvr->addEvent(fd, EPOLLIN | EPOLLET, fd);
		}

		index = (index + 1) % 10;
	}
}


int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout << "Usage: " << argv[0] << " thread_num num_per_thread" <<endl;
		return -1;
	}

	CLogger::init("log4cplus.properties");
	CDebugLogger::init("log4cplus.properties");

	int thread_num = atoi(argv[1]);
	int per_thread= atoi(argv[2]);

	pEventSvr = new EventServer(20480);
	pEventSvr->start();
	
	WorkerThread* workers = new WorkerThread[thread_num];
	for(int i = 0; i < thread_num; ++i)
	{
		workers[i] = WorkerThread();
		workers[i].init(per_thread);
		workers[i].start();
	}

	for(int i = 0; i < thread_num; ++i)
	{
		workers[i].waitForStop();
	}

	pEventSvr->waitForStop();
	
	return 0;
}
