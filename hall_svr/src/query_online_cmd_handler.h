
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


#include "command_handler.h"

class QueryOnlineCmdHandler : public CmdHandler
{
public:
	bool handle(Command * pMsg, vector<sd_job> & out_rsts);	
private:
	bool decodeParam(IDataX* ptr, int& game_id);

private:

	DECL_LOGGER(logger);

};

#endif /*QUERY_ONLINE_CMD_HADLER_H*/

