
#include "shop_config.h"

IMPL_LOGGER(shop_config, logger);

shop_config * shop_config::m_Singleton = NULL;

shop_config::shop_config()
{
	string cfg;
	load_cfg(cfg);
}

shop_config::~shop_config()
{

}

bool shop_config::get_goods_by_type(byte type, vector<goods_item> & goods_set)
{
	goods_set.clear();
	map<int, goods_item>::iterator iter = m_goods_set.begin();
	for ( ; iter != m_goods_set.end(); ++iter )
	{
		if ( (iter->second).tooltype == type )
		{
			goods_set.push_back(iter->second);
		}
	}
	
	return true;
}

bool shop_config::load_cfg(const string & cfg)
{
	bool load_shop_file = m_cfg_reader.load("../conf/shop.conf");
	
	if ( load_shop_file == false )
		return false;
				
	unsigned int shop_cnt = m_cfg_reader.get_int("item_cnt", 0, false);		
	
	for ( unsigned int shop_idx = 0; shop_idx < shop_cnt; ++shop_idx )
	{
		goods_item gold_bean;

		char read_str_buf[32] = {0};
	
		sprintf(read_str_buf, "item%d.toolid", shop_idx);	
		gold_bean.toolid   = m_cfg_reader.get_int(read_str_buf, 0, false);	

		sprintf(read_str_buf, "item%d.toolname", shop_idx);	
		gold_bean.toolname   = m_cfg_reader.get_string(read_str_buf, "", false);	
		
		sprintf(read_str_buf, "item%d.tooltype", shop_idx);
		gold_bean.tooltype   = m_cfg_reader.get_int(read_str_buf, 0, false);

		sprintf(read_str_buf, "item%d.toolicon", shop_idx);
		gold_bean.toolicon   = m_cfg_reader.get_int(read_str_buf, 0, false);

		sprintf(read_str_buf, "item%d.toolnum",  shop_idx);
		gold_bean.toolnum   = m_cfg_reader.get_int(read_str_buf, 0, false);

		sprintf(read_str_buf, "item%d.toolrmb",  shop_idx);
		string tmp_float_str = m_cfg_reader.get_string(read_str_buf, "", false); 
		gold_bean.toolrmb   = atof(tmp_float_str.c_str());

		sprintf(read_str_buf, "item%d.toolkeke",  shop_idx);
		tmp_float_str = m_cfg_reader.get_string(read_str_buf, "", false); 
		gold_bean.toolkeke   = atof(tmp_float_str.c_str());

		sprintf(read_str_buf, "item%d.content",  shop_idx);
		gold_bean.content   = m_cfg_reader.get_string(read_str_buf, "", false);

		sprintf(read_str_buf, "item%d.instore",  shop_idx);
		gold_bean.instore   = m_cfg_reader.get_string(read_str_buf, "", false);

		sprintf(read_str_buf, "item%d.outstore",  shop_idx);
		gold_bean.outstore   = m_cfg_reader.get_string(read_str_buf, "", false);

		sprintf(read_str_buf, "item%d.saleid", shop_idx);
		gold_bean.saleid   = m_cfg_reader.get_int(read_str_buf, 0, false);

		sprintf(read_str_buf, "item%d.takeid", shop_idx);
		gold_bean.takeid   = m_cfg_reader.get_int(read_str_buf, 0, false);

		sprintf(read_str_buf, "item%d.vipvalid", shop_idx);
		gold_bean.vipvalid   = m_cfg_reader.get_int(read_str_buf, 0, false);
																									
		m_goods_set.insert(make_pair(gold_bean.toolid, gold_bean));
		
	}
	
	return true;	
}

