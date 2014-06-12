
/* ======================================================
* 
* file:		hall_svr.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-11 18:09:08
* 
* ======================================================*/

#include <signal.h>
#include <unistd.h>

#include <iostream>

#include "server/application.h"
#include "server/command_registor.h"
#include "common/logger.h"

#include "get_notices_handler.h"
#include "goods_handler.h"
#include "query_room_handler.h"
#include "query_online_handler.h"
#include "notice_config_thread.h"

using namespace std;

Application* g_pApp = NULL;

void regHandler()
{
	CmdRegistor::regCommand("GetNotices", new GetNoticesHandler());
	CmdRegistor::regCommand("QueryOnline", new QueryOnlineCmdHandler());
	CmdRegistor::regCommand("GetDirReq", new QueryRoomHandler());
	CmdRegistor::regCommand("GetStoreConfig", new GoodsHandler());
};

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout << "Usage: " << argv[0] << " /path/app/conf /path/log/conf" <<endl;
		_exit(0);
	}

	//signal(SIGPIPE, SIG_IGN);
	//signal(SIGCHLD, SIG_IGN);
	//signal(SIGHUP, SIG_IGN);
	//signal(SIGINT, SIG_IGN);
	CLogger::init(argv[2]);
	CDebugLogger::init(argv[2]);
	
	
	g_pApp = new Application();

	g_pApp->initialize(argv[1]);

	Configure* pConfig = g_pApp->getConfigure();
	string redis_host = pConfig->getString("redis_ip", "127.0.0.1");
	int redis_port = pConfig->getInt("redis_port", 6379);


	NoticeConfigThread* notice_worker = new NoticeConfigThread();
	notice_worker->init(redis_host, redis_port);
	notice_worker->start();

	g_pApp->waitForShutdown();

	return 0;
}
