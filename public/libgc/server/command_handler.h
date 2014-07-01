
/* ======================================================
* 
* file:		command_handler.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-18 09:55:40
* 
* ======================================================*/

#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <memory>

#include "common/DataXCmd.h"
#include "event.h"

using namespace std;

class ICmdHandler
{
public:
	virtual ~ICmdHandler(){};

	virtual bool handle(CmdTask& task) = 0;

protected:
	int checkCmd(DataXCmdPtr pCmd, const string& cmd_name);

};

inline int ICmdHandler::checkCmd(DataXCmdPtr pCmd, const string& cmd_name)
{
	string name = pCmd->get_cmd_name();
	if(name != cmd_name)
	{
		return -1;
	}

	IDataX* ptr = pCmd->get_datax();
	if(NULL == ptr)
	{
		return -1;
	}

	return 0;
}

typedef shared_ptr<ICmdHandler> ICmdHandlerPtr;

#endif /*COMMAND_HANDLER_H*/
