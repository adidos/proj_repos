
/* ======================================================
* 
* file:		get_notices_handler.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-22 21:19:10
* 
* ======================================================*/
#ifndef GET_NOTICES_HANDLER_H
#define GET_NOTICES_HANDLER_H

#include "command_handler.h"
#include "SDLogger.h"
#include "IDataX.h"
#include "Command.h"

class GetNoticesHandler : public CmdHandler
{
public:
	virtual bool handle(Command * pMsg, vector<sd_job> & out_rsts);

private:
	bool decodeParam(IDataX* pParam, int& game_id, string& channal, short& version);

private:
	DECL_LOGGER(logger);

};

#endif /*GET_NOTICES_HANDLER_H*/
