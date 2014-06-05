#ifndef SHOP_CONFIG_H
#define SHOP_CONFIG_H

#include "../main/common_include.h"
#include "common.h"

class goods_item
{
public:
	goods_item()
	{
		toolid = 0;
		tooltype = 0;
		toolicon = 0;
		toolnum = 0;
		toolrmb = 0;
		toolkeke = 0;
		saleid = 0;
		takeid = 0;
		vipvalid = 0;		
	}

	int    toolid;
	string toolname;
	byte   tooltype;
	short  toolicon;
	int    toolnum;
	float  toolrmb;
	float  toolkeke;
	string content;
	string instore;
	string outstore;
	short  saleid;
	short  takeid;
	byte   vipvalid;
	
private:
	
};

class shop_config
{
public:
	static shop_config * instance()
	{
		if ( m_Singleton == NULL )
		{
			m_Singleton = new shop_config();
		}
		return m_Singleton;
	}

	bool get_goods_by_type(byte type, vector<goods_item> & goods_set);
	
private:	

	shop_config();
	~shop_config();

	bool load_cfg(const string & cfg);	

	static shop_config * m_Singleton;

	map<int, goods_item> m_goods_set;

	ConfigReader         m_cfg_reader;
	
	DECL_LOGGER(logger);	
};

#endif //SHOP_CONFIG_H
