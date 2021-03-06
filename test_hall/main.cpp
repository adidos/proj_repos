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
#include <signal.h>

using namespace std;

#include "common/logger.h"
#include "common/thread.h"
#include "event_server.h"
#include "command_wrapper.h"

EventServer* pEventSvr = NULL;

class WorkerThread : public CThread
{
public:
	WorkerThread():_array(2048)
	{
	}

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
			return -1;
		
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
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int client = socket(AF_INET, SOCK_STREAM, 0);
	int ret = connect(client, (sockaddr*)&addr, sizeof(addr));
	if(ret != 0)
	{
		LOG4CPLUS_DEBUG(ALogger, " connecto to server failed, exit!");

		close(client);
		return -1;
	}
	LOG4CPLUS_DEBUG(ALogger,  client << " connecto to server success!");
	return client;
}

void WorkerThread::doIt()
{
	while(true)
	{
		vector<int>::iterator iter = _array.begin();
		for(;iter != _array.end(); ++iter)
		{
			string buffer = createGetDir(*iter, 1);
			send(*iter, buffer.c_str(), buffer.length(), 0);
			
			usleep(300);
			buffer.clear();
			
			buffer = createGetItem(*iter, 1, 0);
			send(*iter, buffer.c_str(), buffer.length(), 0);
			usleep(300);
		}
	}
}


int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout << "Usage: " << argv[0] << " thread connection" <<endl;
		return -1;
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	CLogger::init("log4cplus.properties");

	int thread_num = atoi(argv[1]);
	int total = atoi(argv[2]);

	pEventSvr = new EventServer(20480);
	pEventSvr->start();
	
	int per_thread = total / thread_num;

	WorkerThread** workers = new WorkerThread*[thread_num];
	for(int i = 0; i < thread_num; ++i)
	{
		workers[i] = new WorkerThread();
		workers[i]->init(per_thread);
		workers[i]->start();
	}

	for(int i = 0; i < thread_num; ++i)
	{
		workers[i]->waitForStop();
	}

	pEventSvr->waitForStop();
	
	return 0;
}
