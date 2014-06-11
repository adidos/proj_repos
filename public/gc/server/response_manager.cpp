#include "response_manager.h"
#include "common/logger.h"

ResponseManager* ResponseManager::_instance = NULL;

ResponseManager::ResponseManager()
	:_task_array(102400)
{
}

int ResponseManager::sendResponse(CmdTask & task)
{
	bool ret = 	_task_array.push(task, 50);
	if(!ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "push response to queue failed."
			<< " response queue current size is " << _task_array.getSize());
	}

	return (ret ? 0 : -1);
}

bool ResponseManager::getRespTask(CmdTask & task)
{
	return _task_array.pop(task, 60*1000);
}
