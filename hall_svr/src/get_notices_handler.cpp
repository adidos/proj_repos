
/* ======================================================
* 
* file:		get_notices_handler.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-04-22 21:22:17
* 
* ======================================================*/
#include "get_notices_handler.h"
#include "notice_config.h"

IMPL_LOGGER(GetNoticesHandler, logger);

bool GetNoticesHandler::handle(Command * pMsg, vector<sd_job> & out_rsts)
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

	int rst = checkCmd(pCmd, string("GetNotices")); 
	if(0 != rst)
	{
		LOG4CPLUS_ERROR(logger, "ckeck command failed. ident = "
			<< pCmd->get_userid() << ", cmd_name = " << pCmd->get_cmd_name());

		return false;
	}

	int game_id = 0;
	string channal;	
	short version;
	bool bSuccess = decodeParam(pCmd->get_datax(), game_id, channal, version);
	if(!bSuccess)
	{
		LOG4CPLUS_ERROR(logger, "decode param from idatax failed.");
		return false;
	}

	LOG4CPLUS_DEBUG(logger, "Get Notices: game_id :" << game_id << ", channal: " << channal << ", version: " << version);

	vector<Notice> notice_array;
	NoticeConfig* ptr = NoticeConfig::getInstance();
	ptr->getNotices(channal, version, notice_array);
	LOG4CPLUS_DEBUG(logger, "get notice cnt =" << notice_array.size());
	
	int idx = 0;
	int size = notice_array.size();

	IDataX* pParam = new XLDataX();
	pParam->PutInt(DataID_GameId, game_id);
	XLDataX** notice_data = new XLDataX*[size];

	vector<Notice>::iterator iter = notice_array.begin();
	for(; iter != notice_array.end(); ++iter)
	{
		notice_data[idx] = new XLDataX();
		notice_data[idx]->PutInt(DataID_NoticeId, iter->id);
		notice_data[idx]->PutUTF8String(DataID_ChanId, (byte*)channal.c_str(),
				channal.length());

		notice_data[idx]->PutInt(DataID_NoticeType,iter->type);
		notice_data[idx]->PutUTF8String(DataID_NoticeTitle,(byte*)iter->title.c_str()
				, iter->title.length());

		notice_data[idx]->PutShort(DataID_NoticeVer, iter->version);
		notice_data[idx]->PutUTF8String(DataID_Content,(byte*)iter->content.c_str()
				, iter->content.length());

		++idx;
		LOG4CPLUS_INFO(logger, "Notice info: { id :" << iter->id << ", type: " 
				<< iter->type << ", version: " << iter->version << ", title: "
				<< iter->title << ", content: " << iter->content);
	}
	pParam->PutInt(DataID_GameId, 1);
	pParam->PutDataXArray(DataID_Param1, (IDataX**)notice_data, size, true);

	DataXCmd* pResp = new DataXCmd("GetNoticesResp", pCmd->get_cipher_flag());
	pResp->set_datax(pParam);
	pResp->set_userid(pCmd->get_userid());

	//TODO response
	message_position msg_pos;
	sd_job rst_job(msg_pos, msg_pos, pResp);
	out_rsts.push_back(rst_job);

	return true;
}


bool GetNoticesHandler::decodeParam(IDataX* pParam, int& game_id, 
			string& channal, short& version)
{
	if(NULL == pParam) return false;
	
	DxWrapper wrapper(pParam);
	
	game_id = wrapper.GetInt(DataID_GameId);
	channal = wrapper.GetUTF8String(DataID_ChanId);
	version = wrapper.GetShort(DataID_NoticeVer);	
	
	return true;	
}
