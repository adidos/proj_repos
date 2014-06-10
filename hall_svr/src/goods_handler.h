#ifndef GOODS_HANDLER_H
#define GOODS_HANDLER_H 

#include "server/IDataX.h"
#include "server/command_handler.h"
#include "server/event.h"

class goods_handler : public CmdHandler
{
public:
	bool handle(CmdTask& task);

private:
	bool decodeParam(IDataX * ptr, int & game_id, int & tooltype);

};

#endif // GOODS_HANDLER_H

