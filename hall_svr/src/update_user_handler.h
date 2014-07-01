
/* ======================================================
* 
* file:		update_user_handler.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-16 11:48:51
* 
* ======================================================*/
#ifndef UPDATE_USER_HANDLER_H
#define UPDATE_USER_HANDLER_H

#include <map>
#include <memory>

#include "server/command_handler.h"
#include "common/IDataX.h"
#include "server/event.h"

class UpdateUserHandler : public ICmdHandler
{
public:
	
	/**
	* brief:
	*
	* @param task
	*
	* @returns   
	*/
	virtual bool handle(CmdTask& task);

private:
	int doUpdateUserBasic(int64_t uid, const string& value);
	
	bool decodeParam(IDataX* ptr, string& value);
};

typedef std::shared_ptr<UpdateUserHandler> UpdateUserHandlerPtr;

#endif //UPDATE_USER_INFO_HANDLER_H
