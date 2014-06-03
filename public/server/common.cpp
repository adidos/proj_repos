
/* ======================================================
* 
* file:		common.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-02 23:56:32
* 
* ======================================================*/

#include "common.h"

/**
* brief:
*
* @param str
*
* @returns   
*/
string trim_left_blank(const string &str)
{
	size_t pos = 0;
	while(pos != str.size())
	{
		if(!isblank(str[pos]))
		{
			break;
		}

		++pos;
	}

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
	size_t pos = str.size() - 1;
	while(pos >= 0)
	{
		if(!isblank(str[pos]))
		{
			break;	
		}
		--pos;
	}

	return str.substr(0, pos + 1);
}


bool isDigit(const string &str)
{
	if(str.empty()) return false;
	
	size_t pos = 0;
	while(pos < str.size())
	{
		if(!isdigit(str[pos]))
			return false;
	}

	return true;
}

bool isDecimal(const string &str)
{
	if(str.empty()) return false;

	int num = 0;
	size_t pos = 0;
	while(pos < str.size())
	{
		if(!isdigit(str[pos]) ||
			str[pos] != '.')
			return false;

		if(str[pos] == '.')
			++num;

		if(num > 1)
			return false;
	}

	return true;
}

int setNoBlock(int fd)
{
	int val = fcntl(fd, F_GETFL);
	if(-1 == val)
	{
		return -1;
	}

	return fcntl(fd, F_SETFL, val | O_NONBLOCK);
}

