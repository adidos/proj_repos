
/* ======================================================
* 
* file:		command_registor.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-02 08:58:11
* 
* ======================================================*/

#ifndef COMMAND_REGISTOR_H
#define COMMAND_REGISTOR_H

#include <string>
#include <map>

#include "command_handler.h"

using namespace std;

class CmdRegistor
{
public:
	static ICmdHandlerPtr getCommand(const string& name);

	static void regCommand(const string& name, ICmdHandlerPtr pHandler);

private:
	typedef map<string, ICmdHandlerPtr>::iterator Iterator;
	static map<string, ICmdHandlerPtr> _handler_array;
};

#endif //COMMAND_REGISTOR_H
