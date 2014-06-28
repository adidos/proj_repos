/* ======================================================
* 
* file:		checkin_handle.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-23 10:31:59
* 
* ======================================================*/

#ifndef CHECK_IN_HANDLE_H
#define CHECK_IN_HANDLE_H

#include <time.h>

#include <map>
#include <string>
#include <vector>
#include <sstream>

#include "server/command_handler.h"
#include "common/IDataX.h"
#include "common/XLDataX.h"
#include "server/event.h"

#include "user_proxy.h"

using namespace std;

struct Item
{
	string id;
	string desc;
	int num;

	string dump()
	{
		ostringstream oss;
		oss << "{id: " << id << ", desc: " 
			<< desc << ", num: " << num <<"}";

		return oss.str();
	}
};


class CheckinHandle : public ICmdHandler
{
public:
	CheckinHandle(): _rewards(NULL)
	{
		srand(time(NULL));
			
		initRewards();
	}

	~CheckinHandle()
	{
		if(NULL != _rewards)
		{
			delete []_rewards;
		}
	}
	
	/**
	* brief:
	*
	* @param task
	*
	* @returns   
	*/
	virtual bool handle(CmdTask& task);

private:
	
	/**
	* brief:
	*
	* @param ptr
	* @param game_id
	* @param chanid
	* @param version
	*
	* @returns   
	*/
	bool decodeParam(IDataX* ptr, int& game_id, string& chanid, short& version);

	/**
	* brief: 
	*
	* @param game_id
	* @param uid
	* @param ret
	*
	* @returns   
	*/
	int doCheckin(int game_id ,int64_t uid, CheckinResult& ret);

	/**
	* brief:
	*
	* @param result
	* @param resp
	*
	* @returns   
	*/
	int handleCheckin(CheckinResult result, UpdateGameInfoResp& resp);

	
	/**
	* brief:
	*
	* @returns   
	*/
	int initRewards();

private:
	XLDataX** _rewards;	

	map<int, Item> _rewards_array;

	int _size;
	
};

#endif //CHECK_IN_HANDLE_H
