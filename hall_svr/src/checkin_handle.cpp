
/* ======================================================
* 
* file:		checkin_handle.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-23 10:33:56
* 
* ======================================================*/

#include "checkin_handle.h"
#include "common/logger.h"
#include "common/DataXCmd.h"
#include "common/XLDataX.h"
#include "common/data_id_const.h"
#include "common/configure.h"
#include "common/utility.h"
#include "server/response_manager.h"
#include "proxy_client.h"


bool CheckinHandle::handle(CmdTask& task)
{
	DataXCmdPtr& pCmd = task.pCmd;
	if(NULL == pCmd)
	{
		LOG4CPLUS_ERROR(FLogger, "convert command to dataxcmd failed.");

		return false;
	}

	int ret = checkCmd(pCmd, string("CheckInReq")); 
	if(0 != ret )
	{
		LOG4CPLUS_ERROR(FLogger, "ckeck command failed. user id = "
			<< pCmd->get_userid() << ", cmd_name = " << pCmd->get_cmd_name());

		return false;
	}

	int64_t uid = pCmd->get_userid();

	int game_id = 1;
	string chan_id ;
	short version = 0;

	bool rst = decodeParam(pCmd->get_datax(), game_id, chan_id, version);
	if(!rst)
	{
		LOG4CPLUS_ERROR(FLogger, "decode param failed");
		return false;
	}

	////向usersvr请求签到
	CheckinResult checkin_ret;

	ret = doCheckin(game_id, uid, checkin_ret);

	/////
	XLDataX* pParam = new XLDataX();
	
	if(0 == ret && 0 == checkin_ret.result)
	{
		//根据签到结果，增加奖励 
		UpdateGameInfoResp update_resp;
		ret = handleCheckin(checkin_ret, update_resp);
	
		if(0 == ret && 0 == update_resp.result)
		{
			pParam->PutInt(DataID_Result, checkin_ret.result);
			pParam->PutShort(DataID_CheckDay, checkin_ret.check_day);
			pParam->PutInt(DataID_BeanNum, update_resp.bean_num);
			pParam->PutDataXArray(DataID_Param1, (IDataX**)_rewards, _size);
		}
		else
		{
			pParam->PutInt(DataID_Result, -2); //签到成功，增加金豆失败
			
			LOG4CPLUS_ERROR(FLogger, "Uesr " << uid << " checkin success, but"
				<< " update the reward failed!");
		}
	}
	else
	{
		int temp = (1 == checkin_ret.result) ? 1 : -1;
		pParam->PutInt(DataID_Result, temp);
	}

	DataXCmdPtr pResp(new DataXCmd("CheckInResp", pCmd->get_cipher_flag()));
	pResp->set_userid(pCmd->get_userid());
	pResp->set_datax(pParam);

	CmdTask resp;
	resp.idx = task.idx;
	resp.seqno = task.seqno;
	resp.pCmd = pResp;
	resp.timestamp = task.timestamp;
	ResponseManager::getInstance()->sendResponse(resp);

	return true;
}


/**
* brief:
*
* @returns   
*/
int CheckinHandle::doCheckin(int game_id ,int64_t uid, CheckinResult& ret)
{
	ProxyClient* ptr = ProxyClient::Instance();

	UserProxy* proxy = (UserProxy*)ptr->getServantPrxy("usersvr");

	if(NULL == proxy)
	{
		LOG4CPLUS_ERROR(FLogger, "User Proxy is NULL!");

		return -1;
	}

	int result = proxy->checkin(game_id, uid, ret);
	
	LOG4CPLUS_ERROR(FLogger, "user checkin call result is  " << result 
		<< ", return " << ret.dump());

	return result;
}

/**
* brief:
*
* @param result
*
* @returns   
*/
int CheckinHandle::handleCheckin(CheckinResult result, UpdateGameInfoResp& resp)
{
	if(0 != result.result)
		return 0;

	ProxyClient* ptr = ProxyClient::Instance();

	UserProxy* proxy = (UserProxy*)ptr->getServantPrxy("usersvr");

	if(NULL == proxy)
	{
		LOG4CPLUS_ERROR(FLogger, "User Proxy is NULL!");

		return -1;
	}

	//会员签到双倍
	int bean = _rewards_array[result.check_day].num;
	if(result.vip_flag > 0)  bean *= 2;

	UpdateGameInfoReq req;
	req.game_id = 1;
	req.win_inc = 0; 
	req.lose_inc = 0;
	req.bean_inc = bean;

	int ret = proxy->updateGameInfo(result.uid ,req, resp);

	LOG4CPLUS_DEBUG(FLogger, result.dump() << " do check in request"
		<< req.dump() << ", result is " << ret << ", response is" << resp.dump());	

	return ret;
}

bool CheckinHandle::decodeParam(IDataX* ptr, int& game_id, string& chanid, short& version)
{
	if(NULL == ptr) return false;

	bool ret = ptr->GetInt(DataID_GameId, game_id);
	if(!ret) return ret;

	int length = 0;
	ret = ptr->GetUTF8String(DataID_ChanId, NULL, length);
	if(!ret) return ret;

	chanid.reserve(length);
	ret = ptr->GetUTF8String(DataID_ChanId, (byte*)chanid.c_str(), length);
	if(!ret) return ret;
	
	ret = ptr->GetShort(DataID_Version, version);

	return ret;
}



/**
* brief:
*
* @returns   
*/
int CheckinHandle::initRewards()
{
	Configure config("../conf/rewards.conf");

	string check_days = config.getString("check_days");

	vector<string> days;
	split_string(check_days, ",", days);	

	_size = days.size();
	_rewards = new XLDataX*[_size];

	LOG4CPLUS_DEBUG(FLogger, "check reward configue: ");
	for(size_t i = 0; i < days.size(); ++i)
	{
		int value = atoi(trim_string(days[i]).c_str());
		
		ostringstream oss;
		oss << "reward_" << value;

		string reward = trim_string(config.getString(oss.str()));
		
		XLDataX* ptr = new XLDataX;
		
		ptr->PutInt(DataID_CheckDay, value);
		ptr->PutUTF8String(DataID_Reward, (byte*)reward.c_str(), reward.length());
		
		_rewards[i] = ptr;

		LOG4CPLUS_DEBUG(FLogger, "{" << value << " : " << reward << "}");

		vector<string> temp;
	
		//TODO 
		split_string(reward, "|", temp);

		if(temp.size() < 3)
			continue;

		Item item;
		item.id = temp[0];
		item.desc = temp[1];
		item.num = atoi(temp[2].c_str());

		LOG4CPLUS_DEBUG(FLogger, item.dump());	

		_rewards_array[value] = item;
	}
	
	return 0;
}
