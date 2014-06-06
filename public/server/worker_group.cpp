
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

WorkerGroup::WorkerGroup()
	: _worker_num(1)
{

}

WorkerGroup::~WorkerGroup()
{
	for(int i= 0; i < _worker_num; ++i)
	{
		if(NULL != _worker_array[i])
		{
			delete _worker_array[i];
			_worker_array[i] = NULL;
		}
	}

	_worker_array.clear();
}

void WorkerGroup::init(int work_num)
{
	for(int i = 0; i < _worker_num; ++i)
	{
		CmdWorker* pWorker = new CmdWorker();
		_worker_array.push_back(pWorker);
	}
}

int WorkerGroup::startWorker()
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

bool WorkerGroup::dispatch(CmdTask& task)
{
	return _worker_array[task.seqno%_worker_num].addTask(task);
}


