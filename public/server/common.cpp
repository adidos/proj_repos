
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


