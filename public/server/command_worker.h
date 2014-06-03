
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

class CmdWorker
{
public:
	CmdWorker(int size);
	~CmdWorker();

protected:
	virtual void doIt();

private:
	Queue<DataXCmd*> _cmd_queue;

};

#endif //COMMAND_WORKER_H
