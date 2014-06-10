#ifndef RESPONSE_MANAGER_H
#define RESPONSE_MANAGER_H

#include "event.h"
#include "common/queue.h"

class ResponseManager
{
public:
	ResponseManager();
	~ResponseManager();

	static ResponseManager* getInstance()
	{
		if(NULL == _instance)
		{
			_instance = new ResponseManager()
		}

		return _instance;
	}

	int sendResponse(CmdTask& task);

	bool getRespTask(CmdTask& task);

private:
	Queue<CmdTask> _task_array;

	static ResponseManager* _instance;

};

#endif //RESPONSE_MANAGER_H