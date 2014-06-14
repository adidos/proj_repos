
/* ======================================================
* 
* file:		game_room.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-13 15:51:41
* 
* ======================================================*/

#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H

struct GameRoom
{
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
};

struct Notice
{
	int id;
	int type;
	string title;
	string content;
	int version;
	vector<string> channal_list;
	string begin_date;
	string end_date;
};

struct GoodsItem 
{
	int    toolid;
	string toolname;
	char   tooltype;
	short  toolicon;
	int    toolnum;
	float  toolrmb;
	float  toolkeke;
	string content;
	string instore;
	string outstore;
	short  saleid;
	short  takeid;
	char   vipvalid;
};

#endif //COMMON_DEFINE_H
