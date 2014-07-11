#include "goods_handler.h"
#include "shop_config.h"
#include "common/logger.h"
#include "server/response_manager.h"
#include "common/data_id_const.h"
#include "common/XLDataX.h"

bool GoodsHandler::handle(CmdTask& task)
{
	DataXCmdPtr& pCmd = task.pCmd;	

	int rst = checkCmd(pCmd, string("GetStoreConfig")); 
	if(0 != rst)
	{
		LOG4CPLUS_ERROR(FLogger, "ckeck command failed. user id = "
			<< pCmd->get_userid() << ", cmd_name = " << pCmd->get_cmd_name());

		return false;
	}

	int game_id  = 0;
	int tooltype = 0;
	bool bSuccess = decodeParam(pCmd->get_datax(), game_id, tooltype);
	if(!bSuccess)	
	{
		LOG4CPLUS_ERROR(FLogger, "decodeParam failed...");
		return false;
	}

	vector<goods_item> goods_set;

	shop_config::instance()->get_goods_by_type(tooltype, goods_set);	
	LOG4CPLUS_DEBUG(FLogger, "get_goods_by_type = " << goods_set.size());

	int idx = 0;
	int size = goods_set.size();

	IDataX* pParam = new XLDataX();
	XLDataX** datax_goods = new XLDataX*[size];
	
	vector<goods_item>::iterator it = goods_set.begin();
	
	for(; it != goods_set.end(); ++it)
	{		
		datax_goods[idx] = new XLDataX();

		datax_goods[idx]->PutInt(DataID_ToolId, it->toolid);
		datax_goods[idx]->PutUTF8String(DataID_ToolName, (byte*)it->toolname.c_str(), it->toolname.length());
		datax_goods[idx]->PutBytes(DataID_ToolType, (byte*)&(it->tooltype), 1);
		datax_goods[idx]->PutShort(DataID_ToolIcon, it->toolicon);
		datax_goods[idx]->PutInt(DataID_ToolNum, it->toolnum);
		datax_goods[idx]->PutFloat(DataID_Rmb, it->toolrmb);
		datax_goods[idx]->PutFloat(DataID_Keke, it->toolkeke);
		datax_goods[idx]->PutUTF8String(DataID_Content, (byte*)it->content.c_str(), it->content.length());
		datax_goods[idx]->PutBytes(DataID_InStore, (byte*)it->instore.c_str(), it->instore.length());
		datax_goods[idx]->PutBytes(DataID_OutStore, (byte*)it->outstore.c_str(), it->outstore.length());
		datax_goods[idx]->PutShort(DataID_SaleId, it->saleid);
		datax_goods[idx]->PutShort(DataID_TaskId, it->takeid);
		datax_goods[idx]->PutBytes(DataID_VIPValid, (byte*)&(it->vipvalid), 1);
		
		++idx;
	}
	pParam->PutInt(DataID_GameId, 1);
	pParam->PutDataXArray(DataID_Param1, (IDataX**)datax_goods, size, true);

	DataXCmdPtr pResp(new DataXCmd("GetStoreConfigResp", pCmd->get_cipher_flag()));
	pResp->set_datax(pParam);
	pResp->set_userid(pCmd->get_userid());

	delete [] datax_goods;
	datax_goods = NULL;

	CmdTask resp;
	resp.idx = task.idx;
	resp.seqno = task.seqno;
	resp.timestamp = task.timestamp;
	resp.pCmd = pResp;
	ResponseManager::getInstance()->sendResponse(resp);	
	
	return true;
}


bool GoodsHandler::decodeParam(IDataX * ptr, int& game_id, int & tooltype)
{
	if(NULL == ptr) return false;

	bool rst = ptr->GetInt(DataID_GameId, game_id);
	rst = ptr->GetInt(DataID_ToolType, tooltype);
	LOG4CPLUS_DEBUG(FLogger, "decodeParam GoodsHandler game_id = "<< game_id << " tooltype = " << tooltype);
	
	return rst;
}
