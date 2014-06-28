
/* ======================================================
* 
* file:		user_proxy.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-18 11:25:29
* 
* ======================================================*/

#ifndef USER_PROXY_H
#define USER_PROXY_H

#include <stdint.h>

#include <map>
#include <string>
#include <sstream>

#include "common/DataXCmd.h"
#include "proxy/servant_proxy.h"

using namespace std;

class IDataX;

struct CheckinResult
{
	int result;
	int64_t uid;
	short check_day;
	short vip_flag;

	string dump()
	{
		ostringstream oss;
		oss << "{user :" << uid << ", checkin result :"
			<< result << ", checkin day : " << check_day
			<< ", vip: " << vip_flag <<" }";

		return oss.str();
	}
};

struct VIPInfoResp
{
	int result;
	int64_t uid;
	short vip_level;
	int	valid_time;

	string dump()
	{
		ostringstream oss;
		oss << "{ user:" << uid << ", vip:" << vip_level
			<< ", valid time:" << valid_time << ", retuslt :"
			<< result << "}";

		return oss.str();
	}
};

struct UpdateGameInfoReq 
{
	int game_id;
	int win_inc;
	int lose_inc;
	int bean_inc;
	
	UpdateGameInfoReq()
	{
		game_id = 1;
		win_inc = 0;
		lose_inc = 0;
		bean_inc = 0;
	}

	string dump()
	{
		ostringstream oss;
		oss << "{ gameid :" << game_id << ", win inc:" << win_inc 
			<< ", lose inc:" << lose_inc << ", bean inc:" << bean_inc
			<< "}";

		return oss.str();
	}
};

struct UpdateGameInfoResp
{
	int result;
	int game_id;
	int bean_num;
	int win_num;
	int lose_num;

	string dump()
	{
		ostringstream oss;
		oss << "{result :" << result << "gameid :" << game_id 
			<< ", bean num:" << bean_num<< ", win time:" << win_num 
			<< ", lose time:" << lose_num << "}";
		return oss.str();
	}
};

class UserProxy : public ServantProxy
{
public:
	
	/**
	* constructor
	*/
	UserProxy(int64_t id)
		:ServantProxy(id)
	{
	}

	/**
	* descontructor
	*/
	~UserProxy(){};

	/**
	* brief:
	*
	* @param uid
	* @param infos
	* @param result
	*
	* @returns   
	*/
	int updateUserBasic(int64_t uid, const string& value, int& result);

	/**
	* brief:
	*
	* @param uid
	* @param info
	* @param ret
	*
	* @returns   
	*/
	int updateGameInfo(int64_t uid, const UpdateGameInfoReq& info, UpdateGameInfoResp& resp);

	/**
	* brief:
	*
	* @param uid
	* @param vip_level
	* @param vaild_day
	* @param ret
	*
	* @returns   
	*/
	int updateVipInfo(int64_t uid, short vip_level, short valid_day, VIPInfoResp& resp);
	
	/**
	* brief:
	*
	* @param game_id
	* @param uid
	* @param result
	*
	* @returns   
	*/
	int checkin(int game_id, int64_t uid, CheckinResult& resp);

private:
	
	/**
	* brief:
	*
	* @param pData
	* @param result
	*
	* @returns   
	*/
	int decodeResponse(IDataX* pData, CheckinResult& resp);

	int decodeResponse(IDataX* pData, int& result);

	int decodeResponse(IDataX* pData, UpdateGameInfoResp& resp);

	int decodeResponse(IDataX* pData, VIPInfoResp& resp);

};

#endif //USER_PROXY_H
