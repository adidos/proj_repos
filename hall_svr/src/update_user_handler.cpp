
/* ======================================================
* 
* file:		update_user_handler.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-16 14:14:17
* 
* ======================================================*/

#include "update_user_handler.h"
#include "common/logger.h"
#include "common/DataXCmd.h"
#include "common/XLDataX.h"
#include "common/data_id_const.h"
#include "server/response_manager.h"

#include "proxy_client.h"
#include "user_proxy.h"

bool UpdateUserHandler::handle(CmdTask& task)
{
	DataXCmdPtr& pCmd = task.pCmd;
	if(NULL == pCmd)
	{
		LOG4CPLUS_ERROR(FLogger, "convert command to dataxcmd failed.");

		return false;
	}

	int rst = checkCmd(pCmd, string("UpdateUserInfoReq")); 
	if(0 != rst)
	{
		LOG4CPLUS_ERROR(FLogger, "ckeck command failed. user id = "
			<< pCmd->get_userid() << ", cmd_name = " << pCmd->get_cmd_name());

		return false;
	}

	string value;
	bool bSuccess = decodeParam(pCmd->get_datax(), value);
	if(!bSuccess)
	{
		LOG4CPLUS_ERROR(FLogger, "decode param failed");
		return false;
	}

	int64_t uid = pCmd->get_userid();
	
	rst =  doUpdateUserBasic(uid, value);
	
	XLDataX* pParam = new XLDataX();
	pParam->PutInt(DataID_Result, 0);

	DataXCmdPtr pResp(new DataXCmd("UpdateUserInfoResp", pCmd->get_cipher_flag()));
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

int UpdateUserHandler::doUpdateUserBasic(int64_t uid, const string& value)
{
	ProxyClient* ptr = ProxyClient::Instance();

	UserProxy* proxy = (UserProxy*)ptr->getServantPrxy("usersvr");

	if(NULL == proxy)
	{
		LOG4CPLUS_ERROR(FLogger, "User Proxy is NULL!");

		return -1;
	}

	int resp = -1;
	int ret = proxy->updateUserBasic(uid, value, resp);

	if(0 == ret)
	{
		return resp;
	}

	return -1;
}

bool UpdateUserHandler::decodeParam(IDataX* ptr, string& value)
{
	if(NULL == ptr) return false;

	int len = 0;
	bool ret = ptr->GetBytes(DataID_Sex, NULL, len);
	if(!ret)
	{
		LOG4CPLUS_ERROR(FLogger, "decode sex data failed.");
		return false;
	}
	
	LOG4CPLUS_DEBUG(FLogger, "param len is " << len);

	string temp(len, 0);
	ret = ptr->GetBytes(DataID_Sex, (byte*)temp.c_str(), len);	
	LOG4CPLUS_DEBUG(FLogger, "decode value :" << temp[0]);

	value = temp;
	
	return ret;
}
