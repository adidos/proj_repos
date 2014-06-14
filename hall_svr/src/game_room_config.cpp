
/* ======================================================
* 
* file:		game_room_config.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-18 15:04:40
* 
* ======================================================*/

#include "game_room_config.h"

#include <stdio.h>
#include "common/logger.h"
#include "common/utility.h"


GameRoomConfig* GameRoomConfig::instance_;


GameRoomConfig::GameRoomConfig()
{

}

/**
* brief: 读取游戏房间配置
*
* @param file_name
*
* @returns   
*/
int GameRoomConfig::loadGameConfig(const string& file_name)
{
	int rst = config_.loadFile(file_name.c_str());
	if(0 != rst)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "load config file " << file_name << " failed...");
		return -1;
	}

	vector<RoomIdent> room_array;
	room_list(room_array);
	
	room_config(room_array, room_list_);
	
	return 0;
}

/**
* brief: 获取房间配置信息
*
* @param game_id 游戏id
* @param room_list[out] 游戏房间配置信息
*
* @returns   
*/
int GameRoomConfig::getRoomConfig(int game_id, vector<GameRoomInfo>& room_list)
{
	vector<GameRoomInfo>::iterator iter = room_list_.begin();
	for(; iter != room_list_.end(); ++iter)
	{
		if(iter->game_id == game_id)
		{
			room_list.push_back(*iter);
		}
	}

	return 0;
}

/**
* brief: 获取游戏房间在线人数
*
* @param game_id 游戏id
* @param online_num 游戏房间在线人数
*
* @returns   
*/
int GameRoomConfig::getOnlineInfo(int game_id, map<RoomIdent, int>& online_num)
{
	map<RoomIdent, int>::iterator iter = num_in_room_.begin();
	for(; iter != num_in_room_.end(); ++iter)
	{
		if(game_id == iter->first.game_id)
		{			
			online_num.insert(*iter);
		}
	}

	return 0;
}

/**
* brief: 更新游戏房间在线人数
*
* @param room  房间标示
* @param change_num  人数改变量，可﹢可﹣ 
*
* @returns   
*/
int GameRoomConfig::updateOnline(const RoomIdent & room, int change_num)
{
	CScopeGuard guard(mutex_);
	num_in_room_[room] += change_num;

	return 0;
}

/**
* brief: 解析游戏id，房间id
*
* @param room_array [out] 房间标示队列
*
* @returns   
*/
int GameRoomConfig::room_list(vector<RoomIdent>& room_array)
{
	vector<int> room_id_array;
	vector<int> game_id_array;

	string strGames = config_.getString("valid.game_id.list");
	if(strGames.empty())
	{
		LOG4CPLUS_ERROR(CLogger::logger, "game list is not config.");
		return -1;
	}

	parse_int_list(strGames, game_id_array);
	
	typedef vector<int>::iterator Iterator;

	Iterator game_iter = game_id_array.begin();
	for(; game_iter != game_id_array.end(); ++game_iter)
	{
		char rooms_key[64] = {'\0'};
		snprintf(rooms_key, 64, "game_%d.room_id.list", *game_iter);	
		
		string strRooms = config_.getString(rooms_key);
		if(strRooms.empty())
		{
			LOG4CPLUS_ERROR(CLogger::logger, "game "<< *game_iter<< " rooms list is null");
			continue;
		}

		parse_int_list(strRooms,room_id_array);
		Iterator room_iter = room_id_array.begin();
		for(; room_iter != room_id_array.end(); ++room_iter)
		{
			RoomIdent room;
			room.game_id = *game_iter;
			room.room_id = *room_iter;
			room_array.push_back(room);
		}
		
		room_id_array.clear();
	}

	return 0;
}

/**
* brief 根据房间标示，读取房间的配置信息
*
* @param room_array[in] 房间标示列表
* @param room_info_array [out] 房间配置信息列表  
*
* @returns   
*/
int GameRoomConfig::room_config(vector<RoomIdent> room_array, 
		vector<GameRoomInfo>& room_info_array)
{
	typedef vector<RoomIdent>::iterator Iterator;
	
	Iterator iter = room_array.begin();	
	for(; iter != room_array.end(); ++iter)
	{	
		GameRoomInfo room_info;
		room_info.game_id = iter->game_id;
		room_info.room_id = iter->room_id;

		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: game_id = " << room_info.game_id
			<< ", room_id = " << room_info.room_id);

		char prefix[32] = {'\0'};
		snprintf(prefix, 32, "game_%d.room_%d.", iter->game_id, iter->room_id);

		room_info.room_name = config_.getString(string(prefix).append("name").c_str());
		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: name = " << room_info.room_name );

		room_info.server_cu =config_.getString(string(prefix).append("server.cu").c_str());
		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: server.cu= " << room_info.server_cu);

		room_info.server_ct =config_.getString(string(prefix).append("server.ct").c_str());
		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: server.ct= " << room_info.server_ct);

		room_info.server_cm =config_.getString(string(prefix).append("server.cm").c_str());
		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: server.cm= " << room_info.server_cm);

		room_info.min_limit=config_.getInt(string(prefix).append("min.bean.limit").c_str());
		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: min.limit= "<<room_info.min_limit);

		room_info.max_limit=config_.getInt(string(prefix).append("max.bean.limit").c_str());
		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: max.limit= "<<room_info.max_limit);

		room_info.room_ante = config_.getInt(string(prefix).append("ante").c_str());
		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: ante = "<<room_info.room_ante);

		room_info.min_quick=config_.getInt(string(prefix).append("quick_min").c_str());
		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: quick_min = "<<room_info.min_quick);

		room_info.max_quick=config_.getInt(string(prefix).append("quick_max").c_str());
		LOG4CPLUS_INFO(CLogger::logger, "RoomInfo: quick_max = "<<room_info.max_quick);

		room_info_array.push_back(room_info);
	}

	return 0;
}

void GameRoomConfig::parse_int_list(const string& source, vector<int>& integers)
{
	string trim_src = trim_string(source);
	if(trim_src.empty())
		return;

	if(trim_src.at(0) == '['){
		trim_src = trim_src.substr(1, trim_src.length() - 2);
	}

	vector<string> sub_strs;
	split_string(trim_src, ",", sub_strs);

	set<int> values;
	vector<string>::iterator iter = sub_strs.begin();
	for(; iter != sub_strs.end(); ++iter)
	{
		if((*iter).empty())
			continue;
		
		int int_value = atoi((*iter).c_str());
		values.insert(int_value);
	}

	for(set<int>::iterator it = values.begin(); it != values.end(); it++)
	{
		integers.push_back(*it);
	}
}

