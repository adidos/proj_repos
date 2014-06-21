
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

bool UpdateUserHandler::handle(CmdTask& task)
{
	DataXCmd* pCmd = task.pCmd;
	if(NULL == pCmd)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "convert command to dataxcmd failed.");

		return false;
	}

	int rst = checkCmd(pCmd, string("UpdateUserInfoReq")); 
	if(0 != rst)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "ckeck command failed. user id = "
			<< pCmd->get_userid() << ", cmd_name = " << pCmd->get_cmd_name());

		return false;
	}

	map<short, string> attr_array;
	bool bSuccess = decodeParam(pCmd->get_datax(), attr_array);
	if(!bSuccess)
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode param failed");
		return false;
	}
	
	//TODO call usersvr interface
	XLDataX* pParam = new XLDataX();
	pParam->PutInt(DataID_Result, 0);

	DataXCmd* pResp = new DataXCmd("UpdateUserInfoResp", pCmd->get_cipher_flag());
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

bool UpdateUserHandler::decodeParam(IDataX* ptr, std::map<short, string>& out_array)
{
	if(NULL == ptr) return false;

	int num = 0;
	ptr->GetDataXArraySize(DataID_Param1, num);
	LOG4CPLUS_DEBUG(CLogger::logger, "array num is " << num);

	IDataX* pData;
	for(int i = 0; i < num; ++i)
	{
		ptr->GetDataXArrayElement(DataID_Param1, i, &pData);	
		
		short attr;
		ptr->GetShort(DataID_Attribute, attr);
		
		int len = 0;
		ptr->GetUTF8String(DataID_Value, NULL, len);
		string value(len, 0);
		ptr->GetUTF8String(DataID_Value, (byte*)value.c_str(), len);
		
		LOG4CPLUS_DEBUG(CLogger::logger, "attribute: " << attr 
				<< ", value len : " << len <<", " << value[0]);

		out_array[attr] = value;	
	}
	
	return true;
}
