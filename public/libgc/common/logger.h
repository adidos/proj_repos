
/* ======================================================
* 
* file:		logger.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-24 16:28:51
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

class CLogger
{
public:
	
	static int init(const string& file)
	{
		try
		{
			PropertyConfigurator::doConfigure(file);
		}
		catch(...)
		{
			cerr << "log init exception..." <<endl;
		}
		
		return 0;
	}

	static Logger Instance(const string& name = "")
	{
		Logger logger;
		try
		{
			if(name.empty())
				logger = Logger::getRoot();
			else
				logger = Logger::getInstance(name);
		}
		catch(exception& e)
		{
			cerr << "logger instance exception: " << e.what() <<endl;
		}

		return logger;
	}

};

#define LOGGER(NAME) CLogger::Instance(NAME) 
#define FLogger LOGGER("Frame")

#endif //GC_LOGGER_H
