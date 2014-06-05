
/* ======================================================
* 
* file:		query_room_cmd_handler.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-19 16:14:14
* 
* ======================================================*/
#ifndef ROOM_CONFIG_CMD_HANDLER_H
#define ROOM_CONFIG_CMD_HANDLER_H 

#include "command_handler.h"


class QueryRoomCmdHandler : public CmdHandler
{
public:
	bool handle(Command * pMsg, vector<sd_job> & out_rsts);

private:
	bool decodeParam(IDataX* ptr, int& game_id);

private:

	DECL_LOGGER(logger);

};

#endif /*ROOM_CONFIG_CMD_HANDLER_H*/

