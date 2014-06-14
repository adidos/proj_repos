
/* ======================================================
* 
* file:		utility.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-02 23:56:32
* 
* ======================================================*/

#include "utility.h"
#include <sys/time.h>

const char* SPACE_STRING = " \t\r\n";

/**
* brief:
*
* @param str
*
* @returns   
*/
string trim_left_blank(const string &str)
{
	size_t pos = str.find_first_not_of(SPACE_STRING);
	if(pos == string::npos)
		return string("");

	return str.substr(pos);
}


/**
* brief:
*
* @param str
*
* @returns   
*/
string trim_right_blank(const string &str)
{
	size_t pos = str.find_last_not_of(SPACE_STRING);
	if(pos == string::npos)
		return string("");
	
	return str.substr(0, pos + 1);
}

/**
* brief:
*
* @param str
*
* @returns   
*/
string trim_string(const string & str)
{
	string::size_type bpos = str.find_first_not_of(SPACE_STRING);
	string::size_type epos = str.find_last_not_of(SPACE_STRING);
	
	if(bpos < 0 || epos < 0)
		return "";
	else if(bpos == 0 && epos == str.length() - 1)
		return str;
	else
		return str.substr(bpos, epos - bpos + 1);
}

/**
* brief:
*
* @param source
* @param delimitor
* @param result_array
*
* @returns   
*/
int split_string(const std::string& source, const char* delimitor, std::vector<std::string>& result_array)
{
	if(delimitor == NULL)
		return 0;

	result_array.clear();

	string::size_type startPos = 0;
	bool reachEnd = false;
	while(!reachEnd)
	{
		string::size_type curPos = source.find(delimitor, startPos);
		if(curPos != string::npos)
		{
			result_array.push_back(source.substr(startPos, curPos - startPos));
			startPos = curPos + strlen(delimitor);
		}
		else
		{
			// add the last part
			if(startPos < source.length())
				result_array.push_back(source.substr(startPos));
									
			reachEnd = true;
		}
	}

	return result_array.size();
}

/**
* brief:
*
* @param str
*
* @returns   
*/
bool isDigit(const string &str)
{
	if(str.empty()) return false;
	
	size_t pos = 0;
	while(pos < str.size())
	{
		if(str[0] == '-')
		{
			++pos;
			continue;
		}

		if(!isdigit(str[pos]))
			return false;

		++pos;
	}

	return true;
}

/**
* brief:
*
* @param str
*
* @returns   
*/
bool isDecimal(const string &str)
{
	if(str.empty()) return false;

	int num = 0;
	size_t pos = 0;
	while(pos < str.size())
	{
		if(str[0] == '-')
		{
			++pos;
			continue;
		}

		if(!isdigit(str[pos]) ||
			str[pos] != '.')
			return false;

		if(str[pos] == '.')
			++num;

		if(num > 1)
			return false;
		++pos;
	}

	return true;
}

int64_t current_time_sec()
{
	timeval val;
	gettimeofday(&val, NULL);

	return val.tv_sec;
}

int64_t current_time_ms()
{
	timeval val;
	gettimeofday(&val, NULL);

	int64_t value = val.tv_sec * 1000 + val.tv_usec / 1000;

	return value;
}

int64_t current_time_usec()
{
	timeval val;
	gettimeofday(&val, NULL);

	int64_t value = val.tv_sec * 1000000 + val.tv_usec;

	return value;
}

/**
* brief:
*
* @param fd
*
* @returns   
*/
int setNoBlock(int fd)
{
	int val = fcntl(fd, F_GETFL);
	if(-1 == val)
	{
		return -1;
	}

	return fcntl(fd, F_SETFL, val | O_NONBLOCK);
}

