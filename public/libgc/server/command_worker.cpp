
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
#include "common/logger.h"
#include "common/utility.h"
#include "command_handler.h"
#include "command_registor.h"

CmdWorker::CmdWorker(int size)
	:_task_queue(size)
{

}

CmdWorker::~CmdWorker()
{

}

/**
* brief:
*
* @param task
*/
bool CmdWorker::addTask(CmdTask task)
{
	bool ret = _task_queue.push(task, 100);
	if(!ret)
	{
		LOG4CPLUS_ERROR(FLogger, _id << " push task to queue fail.");
	}

	LOG4CPLUS_INFO(FLogger, _id << " thread current queue size is "
			<< _task_queue.getSize());
	
	return ret;
}

/**
* brief:
*/
void CmdWorker::doIt()
{
	LOG4CPLUS_DEBUG(FLogger, "command worker start work!");

	while(!_terminate)
	{
		CmdTask task;
		bool bret = _task_queue.pop(task, 60*1000);
		if(!bret)
		{
			LOG4CPLUS_DEBUG(FLogger, "task queue is empty! continue wait...");
			continue;
		}

		LOG4CPLUS_INFO(FLogger, "TimeTrace:[" << task.pCmd->get_cmd_name() 
				<< "] out queue spend time " << current_time_usec()- task.timestamp);

		string name = task.pCmd->get_cmd_name();
		ICmdHandlerPtr pHandler = CmdRegistor::getCommand(name);
		if(!pHandler)
		{
			LOG4CPLUS_WARN(FLogger, "couldn't find handler for command["
				<< name << "]");
			
			continue;
		}

		LOG4CPLUS_DEBUG(FLogger, "find a command handler to process command["
				<< name << "]");
		
		pHandler->handle(task);

		LOG4CPLUS_INFO(FLogger, _id << "TimeTrace:[" << task.pCmd->get_cmd_name() 
				<< "] handle spend time " << current_time_usec() - task.timestamp);
		
		LOG4CPLUS_DEBUG(FLogger, task.pCmd->get_cmd_name() <<" references is "
				<< task.pCmd.use_count());

	}
}
