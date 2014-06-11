
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

#include "common/IDataX.h"
#include "server/command_handler.h"
#include "server/event.h"

class QueryRoomHandler : public ICmdHandler
{
public:
	bool handle(CmdTask& task);

private:
	bool decodeParam(IDataX* ptr, int& game_id);

};

#endif /*ROOM_CONFIG_CMD_HANDLER_H*/

