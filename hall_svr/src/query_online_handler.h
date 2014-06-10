
/* ======================================================
* 
* file:		query_online_cmd_handler.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-19 17:31:01
* 
* ======================================================*/
#ifndef QUERY_ONLINE_CMD_HADLER_H
#define QUERY_ONLINE_CMD_HADLER_H


#include "server/IDataX.h"
#include "server/command_handler.h"
#include "server/event.h"

class QueryOnlineCmdHandler : public ICmdHandler
{
public:
	bool handle(CmdTask& task);	
private:
	bool decodeParam(IDataX* ptr, int& game_id);


};

#endif /*QUERY_ONLINE_CMD_HADLER_H*/

