
/* ======================================================
* 
* file:		game_room_config.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-18 11:35:34
* 
* ======================================================*/
#ifndef GAME_ROOM_CONFIG_H
#define GAME_ROOM_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include "SDLogger.h"
#include "ConfigReader.h"
#include "ThreadSync.h"

using namespace std;

struct RoomIdent
{
	int game_id;
	int room_id;
	
	RoomIdent(){};
	
	RoomIdent(const RoomIdent& other)
	{
		game_id = other.game_id;
		room_id = other.room_id;
	}

	RoomIdent& operator=(const RoomIdent& other)
	{
		game_id = other.game_id;
		room_id = other.room_id;
		return *this;
	}

	bool operator < (const RoomIdent& other) const
	{ 
		if(game_id == other.game_id)
			return room_id < other.room_id;

		return game_id < other.game_id;
	}

	bool operator==(const RoomIdent& rhs) 
	{		
		return (this->game_id == rhs.game_id) && (this->room_id == rhs.room_id);
	}

	bool operator!=(const RoomIdent& rhs) 
	{		
		return (this->game_id != rhs.game_id) || (this->room_id != rhs.room_id);
	}

	string dump() const
	{
		stringstream ss;
		ss << "{game: "  << game_id
			<< ", room: " << room_id 
			<< "}";
		return ss.str();
	}
};

typedef struct _GameRoomInfo{
	int game_id;
	int room_id;
	std::string room_name;
	int min_limit;
	int max_limit;
	int online_user_num;
	std::string server_cu;
	std::string server_ct;
	std::string server_cm;
	int room_ante;
	int min_quick;
	int max_quick;
}GameRoomInfo;

class GameRoomConfig
{
public:
	GameRoomConfig();

	int loadGameConfig(const string& file_name);

	int getRoomConfig(int game_id, vector<GameRoomInfo>& room_list);

	int getOnlineInfo(int game_id, map<RoomIdent, int>& online_num);

	int updateOnline(const RoomIdent & room, int add_num);

	static GameRoomConfig* getInstance()
	{
		if(NULL == instance_)
		{
			instance_ = new GameRoomConfig();
			instance_->loadGameConfig("../conf/game_rooms.conf");
		}
		return instance_;
	}
private:

	int room_list(vector<RoomIdent>& room_array);
	int room_config(vector<RoomIdent> room_array, vector<GameRoomInfo>& room_info_array);


	void parse_int_list(const string& source, vector<int>& integers);


private:
	string file_name_;
	
	Configure config_;

	vector<GameRoomInfo> room_list_;

	CMutex mutex_;

	map<RoomIdent, int>	num_in_room_;

	static GameRoomConfig* instance_;

	DECL_LOGGER(logger);
};

#endif /*GAME_ROOM_CONFIG_H*/
