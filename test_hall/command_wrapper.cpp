
/* ======================================================
* 
* file:		command_wrapper.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-13 17:09:51
* 
* ======================================================*/

#include "command_wrapper.h"

#include "common/logger.h"
#include "common/DataXCmd.h"
#include "common/IDataX.h"
#include "common/XLDataX.h"
#include "common/data_id_const.h"

string createGetDir(uint64_t uid, int game_id)
{
	IDataX* pData = new XLDataX();
	pData->PutInt(DataID_GameId, game_id);
	
	DataXCmd* pCmd = new DataXCmd("GetDirReq", 1);
	pCmd->set_userid(uid);
	pCmd->set_datax(pData);

	int length = pCmd->header_length() + pCmd->body_length();
	string buffer(length, '\0');
	
	bool ret = pCmd->encode((byte*)buffer.c_str(), length);
	if(!ret)
	{
		LOG4CPLUS_ERROR(ALogger, "command encode error!");
		return "";
	}

	delete pCmd;
	pCmd = NULL;

	return buffer;
}

string createGetItem(uint64_t uid, int game_id, int type)
{
	IDataX* pData = new XLDataX();
	pData->PutInt(DataID_GameId, game_id);
	pData->PutInt(DataID_ToolType, type);
	
	DataXCmd* pCmd = new DataXCmd("GetStoreConfig", 1);
	pCmd->set_userid(uid);
	pCmd->set_datax(pData);

	int length = pCmd->header_length() + pCmd->body_length();
	string buffer(length, '\0');
	
	bool ret = pCmd->encode((byte*)buffer.c_str(), length);
	if(!ret)
	{
		LOG4CPLUS_ERROR(ALogger, "command encode error!");
		return "";
	}

	delete pCmd;
	pCmd = NULL;

	return buffer;
}

