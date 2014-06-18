
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

int UserProxy::updateUserBasic(int64_t uid, const map<short, string>& infos,  int& result)
{

	return 0;
}

int UserProxy::updateVipInfo(int64_t uid, short vip_level, short vaild_day , int& result)
{
	IDataX* pParam = new XLDataX();
	//pParam->PutInt();
	
	DataXCmd* pCmd = new DataXCmd("UpdateVipReq", CIPHER_MODE_NONE);
	pCmd->set_userid(uid);
	pCmd->set_datax(pParam);

	DataXCmd* pResp = NULL;

	int ret = invoke(pCmd, &pResp);

	if(NULL == pResp) return -1;

	return ret;
}

int UserProxy::checkin(int64_t uid)
{

	return 0;
}