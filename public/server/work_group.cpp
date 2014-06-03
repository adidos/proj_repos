
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
		
	}
}

