
/* ======================================================
* 
* file:		enter_notify_cmd_handler.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-19 16:41:17
* 
* ======================================================*/

#include "enter_notify_cmd_handler.h"


IMPL_LOGGER(EnterNotifyCmdHandler, logger);

bool EnterNotifyCmdHandler::handle(Command * pMsg, vector<sd_job> & out_rsts)
{
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

	int rst = checkCmd(pCmd, string("EnterRoomNotify")); 
	if(0 != rst)
	{
		LOG4CPLUS_ERROR(logger, "ckeck command failed. ident = "
			<< pCmd->get_userid() << ", cmd_name = " << pCmd->get_cmd_name());

		return false;
	}

	__int64 user_id = 0;
	RoomIdent room_info;
	bool bSuccess = decodeParam(pCmd->get_datax(), user_id, room_info);
	if(!bSuccess)
	{
		LOG4CPLUS_ERROR(logger, "decode param from idatax failed.");
		return false;
	}

	GameRoomConfig* pRoomConfig = GameRoomConfig::getInstance();	
	pRoomConfig->updateOnline(room_info, 1);


	//TODO 添加玩家到对应的房间容器中

	//notify message, no response

	return true;
}


bool EnterNotifyCmdHandler::decodeParam(IDataX* pParam, __int64& user_id, RoomIdent& room)
{
	if(NULL == pParam) return false;

	bool rst = pParam->GetInt64(DataID_PlayerId, user_id);
	if(!rst) return rst;	

	rst = pParam->GetInt(DataID_GameId, room.game_id);
	if(!rst) return rst;

	rst = pParam->GetInt(DataID_RoomId, room.room_id);
	
	return rst;
}

