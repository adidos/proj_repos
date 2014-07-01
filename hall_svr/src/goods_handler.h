#ifndef GOODS_HANDLER_H
#define GOODS_HANDLER_H 

#include <memory>

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

typedef std::shared_ptr<GoodsHandler> GoodsHandlerPtr;

#endif // GOODS_HANDLER_H

