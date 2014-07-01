
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

#include "common/IDataX.h"
#include "server/command_handler.h"
#include "server/event.h"

class GetNoticesHandler : public ICmdHandler
{
public:
	virtual bool handle(CmdTask& task);

private:
	bool decodeParam(IDataX* pParam, int& game_id, string& channal, short& version);

};

typedef shared_ptr<GetNoticesHandler> GetNoticesHandlerPtr;

#endif /*GET_NOTICES_HANDLER_H*/
