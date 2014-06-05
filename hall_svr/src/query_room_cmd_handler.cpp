
/* ======================================================
* 
* file:		query_room_cmd_handler.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-19 16:15:22
* 
* ======================================================*/

#include "query_room_cmd_handler.h"
#include <stdlib.h>



IMPL_LOGGER(QueryRoomCmdHandler, logger);

bool QueryRoomCmdHandler::handle(Command * pMsg, vector<sd_job> & out_rsts)
{
	srand((int) time(0));
	unsigned int tmp_user[4] = {0};

		
	tmp_user[0] = (rand() % (1000-800+1))+ 800;
	tmp_user[1] = (rand() % ( 500-300+1))+ 300;
	tmp_user[2] = (rand() % ( 300-200+1))+ 200;
	tmp_user[3] = (rand() % ( 200-100+1))+ 100;
	
	
	if(NULL == pMsg)
	{
		LOG4CPLUS_WARN(logger, "the command to handle is NULL");
		return false;
	}

	DataXCmd* pCmd = (DataXCmd*)pMsg;
	if(NULL == pCmd)
	{
		LOG4CPLUS_ERROR(logger, "convert command to dataxcmd failed.");

		return false;
	}

	int rst = checkCmd(pCmd, string("GetDirReq")); 
	if(0 != rst)
	{
		LOG4CPLUS_ERROR(logger, "ckeck command failed. user id = "
			<< pCmd->get_userid() << ", cmd_name = " << pCmd->get_cmd_name());

		return false;
	}

	int game_id  = 0;
	bool bSuccess = decodeParam(pCmd->get_datax(), game_id);
	if(!bSuccess)	
	{
		LOG4CPLUS_ERROR(logger, "get game id from datax failed...");
		return false;
	}

	//获取房间配置信息
	vector<GameRoomInfo> room_list;
	GameRoomConfig* pRoomConfig = GameRoomConfig::getInstance();
	pRoomConfig->getRoomConfig(game_id, room_list);

	//获取房间在线人数
	map<RoomIdent, int> rooms_online;
	pRoomConfig->getOnlineInfo(game_id, rooms_online);

	int idx = 0;
	int size = room_list.size();

	IDataX* pParam = new XLDataX();
	XLDataX** rooms_data = new XLDataX*[size];

	RoomIdent room;
	room.game_id = game_id;
	vector<GameRoomInfo>::iterator it = room_list.begin();
	for(; it != room_list.end(); ++it)
	{
		room.room_id = it->room_id;

		rooms_data[idx] = new XLDataX();
		rooms_data[idx]->PutInt(DataID_GameId, it->game_id);
		rooms_data[idx]->PutInt(DataID_RoomId, it->room_id);
		rooms_data[idx]->PutUTF8String(DataID_RoomName, (byte*)it->room_name.c_str(), 
						it->room_name.length());
		rooms_data[idx]->PutInt(DataID_MinBeans_Limit, it->min_limit);
		rooms_data[idx]->PutInt(DataID_MaxBeans_Limit, it->max_limit);
		
		
		rooms_data[idx]->PutInt(DataID_RoomOnlineUsers, rooms_online[room] + tmp_user[idx] );
		rooms_data[idx]->PutUTF8String(DataID_ServerCT, (byte*)it->server_ct.c_str(),
						it->server_ct.length());		
		rooms_data[idx]->PutUTF8String(DataID_ServerCM, (byte*)it->server_cm.c_str(),
						it->server_cm.length());		
		rooms_data[idx]->PutUTF8String(DataID_ServerCU, (byte*)it->server_cu.c_str(),
						it->server_cu.length());		
		rooms_data[idx]->PutInt(DataID_RoomAnte, it->room_ante);
		rooms_data[idx]->PutInt(DataID_QuickMin, it->min_quick);
		rooms_data[idx]->PutInt(DataID_QuickMax, it->max_quick);

		++idx;
	}
	pParam->PutDataXArray(DataID_Param1, (IDataX**)rooms_data, size, true);

	DataXCmd * pResp = new DataXCmd("GetDirResp", pCmd->get_cipher_flag());
	pResp->set_datax(pParam);
	pResp->set_userid(pCmd->get_userid());

	
	//send pResp
	message_position msg_pos;
	sd_job rst_job(msg_pos, msg_pos, pResp);
	out_rsts.push_back(rst_job);
	return true;
}


bool QueryRoomCmdHandler::decodeParam(IDataX* ptr, int& game_id)
{
	if(NULL == ptr) return false;

	bool rst = ptr->GetInt(DataID_GameId, game_id);
	
	return rst;
}
