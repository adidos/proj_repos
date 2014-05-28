
/* ======================================================
* 
* file:		logger.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-24 16:28:51
* 
* ======================================================*/

#ifndef DOS_LOGGER_H
#define DOS_LOGGER_H

#include <string>
#include <iostream>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>

using namespace std;
using namespace log4cplus;

class CRootLogger
{
public:
	static int init(const string& file)
	{
		try
		{
			PropertyConfigurator::doConfigure(file);
			logger = Logger::getRoot();	
		}
		catch(...)
		{
			cerr << "log init exception..." <<endl;
		}
		
		return 0;
	}

	static Logger logger;
};

class CDebugLogger
{
public:
	static int init(const string& file)
	{
		try
		{
			PropertyConfigurator::doConfigure(file);
			logger = Logger::getInstance("Dev");	
		}
		catch(...)
		{
			cerr << "log init exception..." <<endl;
		}
		
		return 0;
	}

	static Logger logger;
};

#endif //DOS_LOGGER_H
