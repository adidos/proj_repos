
/* ======================================================
* 
* file:		user_proxy.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-18 11:38:35
* 
* ======================================================*/

#include "user_proxy.h"

#include "common/IDataX.h"
#include "common/XLDataX.h"
#include "common/DataXCmd.h"
#include "common/logger.h"


/**
* brief: 更新用户基本信息
*
* @param uid user id
* @param infos 修改属性列表
* @param result 0更新成功，其他失败
*
* @returns  0成功，其他失败
*/
int UserProxy::updateUserBasic(int64_t uid, const string& value,  int& result)
{
	IDataX* pParam = new XLDataX();
	pParam->PutInt64(DataID_PlayerId, uid);
	pParam->PutBytes(DataID_Sex, (byte*)value.c_str(), 1);
	
	LOG4CPLUS_DEBUG(CLogger::logger, "value: " << value[0]);

	DataXCmdPtr pReq(new DataXCmd("UpdateUserInfoReq", CIPHER_MODE_NONE));
	
	pReq->set_userid(_id);
	pReq->set_datax(pParam);

	DataXCmdPtr pResp;
	
	int ret =invoke(pReq, pResp);

	if(!pResp || 0 != ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "UpdateUserInfoReq response is NULL");

		return -1;
	}

	ret = decodeResponse(pResp->get_datax(), result);

	return ret;
}

/**
* brief:
*
* @param uid
* @param info
* @param ret
*
* @returns   
*/
int UserProxy::updateGameInfo(int64_t uid, const UpdateGameInfoReq& info, UpdateGameInfoResp& resp)
{
	IDataX* pParam = new XLDataX();

	pParam->PutInt(DataID_GameId, info.game_id);
	//pParam->PutInt64(DataID_PlayerId, uid);
	pParam->PutInt(DataID_WinInc, info.win_inc);
	pParam->PutInt(DataID_LoseInc, info.lose_inc);
	pParam->PutInt(DataID_BeanInc, info.bean_inc);

	DataXCmdPtr pCmd(new DataXCmd("UpdateGameInfo", CIPHER_MODE_NONE));
	pCmd->set_userid(uid); //TODO modify this id to server id
	pCmd->set_datax(pParam);

	DataXCmdPtr pResp;

	int ret = invoke(pCmd, pResp);

	if(!pResp || 0 != ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "get response failed!");

		return -1;
	}

	IDataX* pData = pResp->get_datax();
	if(NULL == pData)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "no data in response!");

		return -1;
	}

	ret = decodeResponse(pData, resp);

	return ret;
}

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
int UserProxy::updateVipInfo(int64_t uid, short vip_level, short valid_day, VIPInfoResp& resp)
{
	IDataX* pParam = new XLDataX();

	pParam->PutInt64(DataID_PlayerId, uid);
	pParam->PutShort(DataID_Vip, vip_level);
	pParam->PutShort(DataID_Days, valid_day);
	
	DataXCmdPtr pCmd(new DataXCmd("UpdateVipReq", CIPHER_MODE_NONE));
	pCmd->set_userid(_id);
	pCmd->set_datax(pParam);

	DataXCmdPtr pResp;

	int ret = invoke(pCmd, pResp);

	if(!pResp || 0 != ret) return -1;

	IDataX* pData = pResp->get_datax();

	if(NULL == pData)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "no data in response!");

		return -1;
	}

	ret = decodeResponse(pData, resp);

	return ret;
}
   
/**
* brief:
*
* @param game_id
* @param uid
* @param result
*
* @returns   
*/
int UserProxy::checkin(int game_id, int64_t uid, CheckinResult& resp)
{
	IDataX* pParam = new XLDataX();

	pParam->PutInt(DataID_GameId, game_id);
	pParam->PutInt64(DataID_PlayerId, uid);
	pParam->PutUTF8String(DataID_ChanId, (byte*)"", 0);
	pParam->PutShort(DataID_Version, 0);

	DataXCmdPtr pCmd(new DataXCmd("CheckInReq", CIPHER_MODE_NONE));
	pCmd->set_userid(_id);
	pCmd->set_datax(pParam);

	DataXCmdPtr pResp;

	int ret = invoke(pCmd, pResp);

	if(!pResp || 0 != ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "get response failed!");

		return -1;
	}

	IDataX* pData = pResp->get_datax();
	if(NULL == pData)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "no data in response!");

		return -1;
	}

	ret = decodeResponse(pData, resp);

	return ret;
}


/**
* brief:
*
* @param pData
* @param result
*
* @returns   
*/
int UserProxy::decodeResponse(IDataX* pData, CheckinResult& resp)
{
	if(NULL == pData) return -1;

	bool ret = pData->GetInt(DataID_Result, resp.result);
	
	if(ret && 0 == resp.result)
	{
		ret = pData->GetInt64(DataID_PlayerId, resp.uid);

		if(!ret)
		{
			LOG4CPLUS_ERROR(CLogger::logger, "decode uid from data failed!");
			return -1;
		}

		ret = pData->GetShort(DataID_CheckDay, resp.check_day);

		if(!ret)
		{
			LOG4CPLUS_ERROR(CLogger::logger, "decode check day uid from data failed!");
			return -1;
		}

		ret = pData->GetShort(DataID_Vip, resp.vip_flag);

		if(!ret)
		{
			LOG4CPLUS_ERROR(CLogger::logger, "decode vip flag from data failed!");
			return -1;
		}
	}

	return 0;
}

/**
* brief:
*
* @param pData
* @param result
*
* @returns   
*/
int UserProxy::decodeResponse(IDataX* pData, int& result)
{
	if(NULL == pData) return -1;

	bool ret = pData->GetInt(DataID_Result, result);
	
	if(! ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode response result failed!");	
		
		return -1;
	}

	return 0;
}

/**
* brief:
*
* @param pData
* @param result
*
* @returns   
*/
int UserProxy::decodeResponse(IDataX* pData, VIPInfoResp& resp)
{
	if(NULL == pData) return -1;

	bool ret = pData->GetInt(DataID_Result, resp.result);
	
	if(! ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode update game info result failed!");	
		
		return 0;
	}

	if(0 != resp.result) return 0;

	ret = pData->GetInt64(DataID_PlayerId, resp.uid);

	if(!ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode uid from data failed!");
		return -1;
	}

	ret = pData->GetShort(DataID_Vip, resp.vip_level);

	if(!ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode vip level from data failed!");
		return -1;
	}

	ret = pData->GetInt(DataID_ValidTime, resp.valid_time);

	if(!ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode valid time from data failed!");
		return -1;
	}
	
	return 0;
}

/**
* brief:
*
* @param pData
* @param resp
*
* @returns   
*/
int UserProxy::decodeResponse(IDataX* pData, UpdateGameInfoResp& resp)
{
	if(NULL == pData) return -1;

	bool ret = pData->GetInt(DataID_Result, resp.result);
	
	if(! ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode update game info result failed!");	
		
		return 0;
	}

	if(0 != resp.result) return 0;

	ret = pData->GetInt(DataID_GameId, resp.game_id);

	if(!ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode uid from data failed!");
		return -1;
	}

	//ret = pData->GetInt64(DataID_PlayerId, resp.uid);

	//if(!ret)
	//{
	//	LOG4CPLUS_ERROR(CLogger::logger, "decode uid from data failed!");
	//	return -1;
	//}

	ret = pData->GetInt(DataID_WinNum, resp.win_num);

	if(!ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode win num from data failed!");
		return -1;
	}

	ret = pData->GetInt(DataID_LoseNum, resp.lose_num);

	if(!ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode lose num from data failed!");
		return -1;
	}

	ret = pData->GetInt(DataID_BeanNum, resp.bean_num);

	if(!ret)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode bean num from data failed!");
		return -1;
	}

	return 0;	
}

