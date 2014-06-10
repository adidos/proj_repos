
/* ======================================================
* 
* file:		query_online_cmd_handler.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-19 17:32:26
* 
* ======================================================*/

#include "query_online_cmd_handler.h"
#include "server/response_manager.h"
#include "common/logger.h"

bool QueryOnlineCmdHandler::handle(CmdTask& task)
{
	DataXCmd* pCmd = task.pCmd;
	if(NULL == pCmd)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "convert command to dataxcmd failed.");

		return false;
	}

	int rst = checkCmd(pCmd, string("QueryOnline")); 
	if(0 != rst)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "ckeck command failed. user id = "
			<< pCmd->get_userid() << ", cmd_name = " << pCmd->get_cmd_name());

		return false;
	}

	int game_id  = 0;
	bool bSuccess = decodeParam(pCmd->get_datax(), game_id);
	if(!bSuccess)	
	{
		LOG4CPLUS_ERROR(CLogger::logger, "get game id from datax failed...");
		return false;
	}


	//获取房间在线人数
	map<RoomIdent, int> rooms_online;
	GameRoomConfig* pRoomConfig = GameRoomConfig::getInstance();
	pRoomConfig->getOnlineInfo(game_id, rooms_online);

	int idx = 0;
	int size = rooms_online.size();

	IDataX* pParam = new XLDataX();
	XLDataX** online_data = new XLDataX*[size];

	map<RoomIdent, int>::iterator iter = rooms_online.begin();
	for(; iter != rooms_online.end(); ++iter)
	{
		online_data[idx] = new XLDataX();		
		online_data[idx]->PutInt(DataID_RoomId, iter->first.room_id);
		online_data[idx]->PutInt(DataID_RoomOnlineUsers,iter->second + 200);
		
		LOG4CPLUS_INFO(CLogger::logger, "room online: " << iter->first.dump() 
						<< "\t " << iter->second);
	}
	pParam->PutInt(DataID_GameId, 1);
	pParam->PutDataXArray(DataID_Param1, (IDataX**)online_data, size, true);

	DataXCmd* pResp = new DataXCmd("QueryOnlineResp", pCmd->get_cipher_flag());
	pResp->set_datax(pParam);
	pResp->set_userid(pCmd->get_userid());

	CmdTask resp;
	resp.idx = task.idx;
	resp.seqno = task.seqno;
	resp.pCmd = pResp;
	resp.timestamp = task.timestamp;
	ResponseManager::getInstance()->sendResponse(resp);

	return true;
}


bool QueryOnlineCmdHandler::decodeParam(IDataX* ptr, int& game_id)
{
	if(NULL == ptr) return false;

	bool rst = ptr->GetInt(DataID_GameId, game_id);
	
	return rst;
}
