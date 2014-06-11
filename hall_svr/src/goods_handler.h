#ifndef GOODS_HANDLER_H
#define GOODS_HANDLER_H 

#include "common/IDataX.h"
#include "server/command_handler.h"
#include "server/event.h"

class GoodsHandler : public ICmdHandler
{
public:
	bool handle(CmdTask& task);

private:
	bool decodeParam(IDataX * ptr, int & game_id, int & tooltype);

};

#endif // GOODS_HANDLER_H

