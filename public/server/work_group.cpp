
/* ======================================================
* 
* file:		work_group.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-28 19:15:52
* 
* ======================================================*/

#include "work_group.h"

WorkerGroup::WorkerGroup(int worker_num)
	:_worker_num(worker_num), _worker_array(worker_num),
	_client_mgr_ptr(NULL)
{

}

void WorkerGroup::init()
{
	for(int i = 0; i < _worker_num; ++i)
	{
		CmdWorker* pWorker = new CmdWorker();
		_worker_array.push_back(pWorker);
	}
	
	_client_mgr_ptr = new ClientManager();
}

int WorkerGroup::startWork()
{
	for(int i = 0; i < _worker_num; ++i)
	{
		_worker_array[i].start();
	}
}

int WorkerGroup::waitForStop()
{
	for(int i = 0; i < _worker_num; ++i)
	{
		_worker_array[i].waitForStop();
	}
}

bool WorkerGroup::dispatch(CmdTask task)
{
	int64_t uid = task.pCmd->get_userid();
	if(uid <= 0)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "wrong uid value, " << uid);
		return false;
	}

	int old_sid = _client_mgr_ptr->getSessID(uid);
	if(-1 == sid)
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "client " << uid << " session is not set!");

		_client_mgr_ptr->addClient(uid, task.seqno);
	}
	else if(old_sid != task.seqno)
	{
		DataXCmd *pCmd = new DataXCmd("KickPlayer", CIPHER_MODE_TEA);
		pCmd->set_userid(uid);

		CmdTask resp_task;
		resp_task.idx = 0;
		resp_task.pCmd = pCmd;
		resp_task.seqno = old_sid;
		resp_task.timestamp = current_time_usec();

		//TODO put the command to send thread

		LOG4CPLUS_DEBUG(CLogger::logger, "client " << uid << " relogin, kick old session"
				<< old_sid << ".");

		_client_mgr_ptr->resetClient(uid, task.seqno);
	}

	if("UserDrop" == task.pCmd->get_cmd_name())
	{
		_client_mgr_ptr->freeClient(uid);
	}

	return _worker_array[task.seqno%_worker_num].addTask(task);
}

