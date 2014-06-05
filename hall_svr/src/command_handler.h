
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

#include "Command.h"
#include "IDataX.h"
#include "SDLogger.h"
#include "DataXCmd.h"
#include "data_id_const.h"
#include "game_room_config.h"

class ICmdHandler
{
public:
	virtual bool handle(DataXCmd *pMsg, vector<sd_job> & out_rsts) = 0;

protected:
	int checkCmd(DataXCmd* pCmd, const string& cmd_name);

};

inline int ICmdHandler::checkCmd(DataXCmd* pCmd, const string& cmd_name)
{
	string name = pCmd->get_cmd_name();
	if(name.empty() || name != cmd_name)
	{
		return -1;
	}

	int64_t user_id = pCmd->get_userid();
	if(user_id < 0)
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


#endif /*COMMAND_HANDLER_H*/