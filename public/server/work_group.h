
/* ======================================================
* 
* file:		work_group.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-28 18:28:35
* 
* ======================================================*/

#ifndef WORK_GROUP_H
#define WORK_GROUP_H

#include "event.h"

class CmdWorker;

class WorkerGroup
{
public:
	WorkerGroup();
	~WorkerGroup();

	void init(int work_num);

	int startWorker();

	int waitForStop();

	bool dispatch(CmdTask& task);

private:
	int getIndex();

private:
	int _worker_num;
	
	vector<CmdWorker*> _worker_array;

	

};

#endif //WORK_GROUP_H
