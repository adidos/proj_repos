
/* ======================================================
* 
* file:		gc_logger.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-18 20:05:16
* 
* ======================================================*/

#ifndef GC_LOGGER_H
#define GC_LOGGER_H

#include <string>
#include <iostream>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>

using namespace std;
using namespace log4cplus;

class GCLogger
{
public:
	static int init(const string file)
	{
		try
		{
			PropertyConfigurator::doConfigure(file);
			ROOT = Logger::getRoot();	
		}
		catch(...)
		{
			cerr << "log init exception..." <<endl;
		}
		
		return 0;
	}

	static Logger ROOT;
};

#endif //GC_LOGGER_H
