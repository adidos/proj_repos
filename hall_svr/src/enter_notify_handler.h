
/* ======================================================
* 
* file:		enter_notify_cmd_handler.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-19 16:27:58
* 
* ======================================================*/

#ifndef ENTER_NOTIFY_CMD_HANDLER_H
#define ENTER_NOTIFY_CMD_HANDLER_H

#include "command_handler.h"


class EnterNotifyCmdHandler : public CmdHandler
{
public:
	bool handle(Command * pMsg, vector<sd_job> & out_rsts);

private:
	bool decodeParam(IDataX* pParam, __int64& user_id, RoomIdent& room);

private:
	DECL_LOGGER(logger);

};

#endif /*ENTER_NOTIFY_CMD_HANDLER_H*/
