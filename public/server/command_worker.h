
/* ======================================================
* 
* file:		command_worker.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-03 20:32:45
* 
* ======================================================*/

#ifndef COMMAND_WORKER_H
#define COMMAND_WORKER_H

#include "thread.h"
#include "event.h"

class CmdWorker : public CThread
{
public:
	CmdWorker(int size);
	~CmdWorker();

	bool addTask(CmdTask task);

protected:
	virtual void doIt();

private:
	Queue<CmdTask> _task_queue;

};

#endif //COMMAND_WORKER_H
