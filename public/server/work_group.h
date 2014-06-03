
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

class CmdWorker;

class WorkerGroup
{
public:
	WorkerGroup(int worker_num);
	~WorkerGroup();

	void init();

	bool addTask(DataXCmd* pCmd);


private:
	int _worker_num;
	
	vector<CmdWorker*> _worker_array;
	
	ClientManager* _client_mgr_ptr;
};

#endif //WORK_GROUP_H
