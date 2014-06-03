
/* ======================================================
* 
* file:		command_worker.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-03 20:49:36
* 
* ======================================================*/

#include "command_worker.h"

CmdWorker::CmdWorker()
{

}

CmdWorker::~CmdWorker()
{

}

void CmdWorker::doIt()
{
	while(true)
	{
		DataXCmd* pCmd = NULL;
		bool bret = _cmd_queue.pop(pCmd, 10000);
		if(NULL == pCmd)
		{
			continue;
		}
		
		string name = pCmd->get_cmd_name();
		ICmdHandler* pHandler =	CmdRegistor::getCommand();
		if(NULL == pHandler)
		{
			LOG4CPLUS_WARN(CLogger::logger, "couldn't find handler for command["
				<< name << "]");
			continue;
		}
		
		
	}


}
