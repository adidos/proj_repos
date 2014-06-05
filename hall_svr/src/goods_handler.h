#ifndef GOODS_HANDLER_H
#define GOODS_HANDLER_H 

#include "command_handler.h"

class goods_handler : public CmdHandler
{
public:
	bool handle(Command * pMsg, vector<sd_job> & out_rsts);

private:
	bool decodeParam(IDataX * ptr, int & game_id, int & tooltype);

private:

	DECL_LOGGER(logger);

};

#endif // GOODS_HANDLER_H

