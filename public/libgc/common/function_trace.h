
/* ======================================================
* 
* file:		function_trace.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-24 10:14:06
* 
* ======================================================*/

#ifndef FUNCTION_TRACE_H
#define FUNCTION_TRACE_H

#include <string>

#include "logger.h"

using namespace std;

class FunctionTrace
{
public:
	FunctionTrace(const string& clazz,const string& func)
		:_class_name(clazz), _func_name(func)
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "Enter Function " 
			<< _class_name << ":"<< _func_name);
	}

	~FunctionTrace()
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "Exit Function "
			<< _class_name << ":"<< _func_name);
	}

private:
	string _class_name;
	string _func_name;

};

#define TRACE(clazz ,func) FunctionTrace(clazz, func)

#endif 
