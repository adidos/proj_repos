
/* ======================================================
* 
* file:		configure.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-02 22:54:06
* 
* ======================================================*/

#include "configure.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "utility.h"
#include "logger.h"

Configure::Configure()
{

}

Configure::Configure(const string &file)
{
	loadFile(file);
}

Configure::~Configure()
{
	if(_ifs.is_open())
	{
		_ifs.close();
	}
	_config_items.clear();
}

int Configure::loadFile(const string &file)
{
	if(_ifs.is_open())
	{
		_ifs.close();
	}
	
	_ifs.open(file.c_str(), ios::binary | ios::in);
	if(_ifs.fail() | !_ifs.is_open())
	{
		LOG4CPLUS_ERROR(CLogger::logger, "open file[" << file << "] fail"
			<< ", error : " << strerror(errno));
		return -1;
	}

	_config_items.clear();
	return this->parseConfig();
}

string Configure::getString(const string &key, const string &value)
{
	if(key.empty())
	{
		return value;
	}

	Iterator iter = _config_items.find(key);
	if(iter == _config_items.end())
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "config[" << key << "] is  not "
			<< "exist, so use default value!");
		return value;
	}

	return iter->second;
}

int Configure::getInt(const string &key, const int &value)
{
	if(key.empty())
	{
		return value;
	}

	Iterator iter = _config_items.find(key);
	if(iter == _config_items.end())
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "config[" << key << "] is  not "
			<< "exist, so use default value!");
		return value;
	}

	if(! isDigit(iter->second))
	{
		LOG4CPLUS_ERROR(CLogger::logger, "isDigit check failed for value "
			<< iter->second);
		return value;
	}

	return atoi(iter->second.c_str());
}

bool Configure::getBool(const string &key, const bool &value)
{
	if(key.empty())
	{
		return value;
	}

	Iterator iter = _config_items.find(key);
	if(iter == _config_items.end())
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "config[" << key << "] is  not "
			<< "exist, so use default value!");
		return value;
	}

	string ret = iter->second;
	if(ret.empty())
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "config[" << key << "]'s value is"
			<< " NULL, so use default value!");
		return value;
	}

	//TODO lowwer(ret),then compare true
	if(ret.compare("true") == 0 ||	ret.compare("True") == 0)
		return true;
	
	return false;
}

float Configure::getFloat(const string &key, const float &value)
{
	if(key.empty())
	{
		return value;
	}

	Iterator iter = _config_items.find(key);
	if(iter == _config_items.end())
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "config[" << key << "] is  not "
			<< "exist, so use default value!");
		return value;
	}

	if(! isDecimal(iter->second))
	{
		LOG4CPLUS_ERROR(CLogger::logger, "isDecimal check failed for value "
			<< iter->second);
		return value;
	}

	return (float)atof(iter->second.c_str());
}
int Configure::parseConfig()
{
	if(_ifs.fail())
	{
		return -1;
	}

	string line;
	while(getline(_ifs, line))
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "read line : " << line);
		if(line.empty())
			continue;

		size_t pos = line.find("#");
		if(string::npos != pos)
			line.erase(pos);

		pos = line.find("//");
		if(string::npos != pos)
			line.erase(pos);

		if(line.empty()) 
			continue;

		pos = line.find("=");
		if(pos == string::npos)
			continue;

		string key = line.substr(0, pos);
		string value = line.substr(pos + 1);

		key = trim_left_blank(key);
		key = trim_right_blank(key);

		LOG4CPLUS_DEBUG(CLogger::logger, "key : " << key);

		value = trim_left_blank(value);
		value = trim_left_blank(value);
		
		LOG4CPLUS_DEBUG(CLogger::logger, "value: " << value);

		if(!key.empty())
			_config_items[key] = value;
	}

	return 0;
}

string Configure::dump()
{
	LOG4CPLUS_DEBUG(CLogger::logger, "*********** config item begin ********** ");

	ostringstream oss;
	map<string,string>::iterator iter = _config_items.begin();
	for(; iter != _config_items.end(); ++iter)
	{
		LOG4CPLUS_DEBUG(CLogger::logger, iter->first << " = " << iter->second);
		oss << iter->first << " = " << iter->second << endl;
	}

	LOG4CPLUS_DEBUG(CLogger::logger, "*********** config item end********** ");
	return oss.str();
}



int main()
{
	CLogger::init("log4cplus.properties");
	Configure config("config.txt");
	cout << config.dump();

	return 0;
}
