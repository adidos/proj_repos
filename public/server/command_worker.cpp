
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
#include "logger.h"

CmdWorker::CmdWorker()
	:_task_queue(10240)
{

}

CmdWorker::~CmdWorker()
{

}

void CmdWorker::addTask(CmdTask task)
{
	bool bret = _task_queue.push(task, 100);
	if(!bret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, _id << " push task to queue fail.");
	}
	LOG4CPLUS_DEBUG(CDebugLogger::logger, _id << " thread current queue size is "
			<< _task_queue.getSize());
	
	return bret;
}

void CmdWorker::doIt()
{
	while(true)
	{
		CmdTask task;
		bool bret = _task_queue.pop(task, 60*1000);
		if(!bret)
		{
			LOG4CPLUS_DEBUG(CLogger::logger, "task queue is empty! wait continue...");
			continue;
		}

		LOG4CPLUS_DEBUG(CDebugLogger::logger, _id << "TimeTrace: task->out queue spend time " 
				<< current_time_usec()- task.timestamp);
		string name = task.pCmd->get_cmd_name();
		ICmdHandler* pHandler = CmdRegistor::getCommand();
		if(NULL == pHandler)
		{
			LOG4CPLUS_WARN(CLogger::logger, "couldn't find handler for command["
				<< name << "]");
			continue;
		}

		LOG4CPLUS_DEBUG(CLogger::logger, "find a command handler to process command["
				<< name << "]");
		
		pHandler->handle(task);

		LOG4CPLUS_DEBUG(CDebugLogger::logger, _id << "TimeTrace: task->handle spend time "
				<< current_time_usec() - task.timestamp);
	}


}
