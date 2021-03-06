
/* ======================================================
* 
* file:		command_registor.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-02 09:02:28
* 
* ======================================================*/

#include "command_registor.h"
#include "common/logger.h"

map<string, ICmdHandlerPtr> CmdRegistor::_handler_array;

ICmdHandlerPtr CmdRegistor::getCommand(const string& name)
{
	Iterator iter = _handler_array.find(name);
	if(iter == _handler_array.end())
	{
		return ICmdHandlerPtr();
	}	
	
	return iter->second;
}

void CmdRegistor::regCommand(const string& name, ICmdHandlerPtr pHandler)
{
	pair<Iterator, bool> ret = _handler_array.insert(make_pair(name, pHandler));
	if(!ret.second)
	{
		LOG4CPLUS_WARN(FLogger, "command[" << name << "] is already registed!");
		return;
	}
}

