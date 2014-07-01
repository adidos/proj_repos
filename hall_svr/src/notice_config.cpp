#include "notice_config.h"
#include "common/logger.h"

#include <algorithm>
#include <stdint.h>

NoticeConfig * NoticeConfig::instance_ = NULL;


/**
* brief:
*
* @param notice
*
* @returns   
*/
int NoticeConfig::addNotice(Notice& notice)
{

	vector<Notice>::iterator iter = find(notice_array_.begin(),
			notice_array_.end(), notice);

	if(iter == notice_array_.end())
	{
		LOG4CPLUS_DEBUG(ALogger, "addNotice()");
		notice_array_.push_back(notice);
	}

	return 0;
}

/**
* brief:
*
* @param notice_list
* @param bAppend
*
* @returns   
*/
int NoticeConfig::addNotice(vector<Notice> notice_list, bool bAppend)
{
	LOG4CPLUS_DEBUG(ALogger, "addNotice list " << notice_list.size());
	
	if ( !bAppend )
		notice_array_.clear();

	for (uint32_t i=0; i<notice_list.size(); ++i)
	{
		notice_array_.push_back(notice_list[i]);
	}
	
	return 0;
}


/**
* brief: 请求的渠道和版本在公告的渠道和版本列表中，
*		或者公告的渠道和版本列表为空，返回该公告
* @param channel
* @param verid
* @param out_list
*
* @returns   
*/
int NoticeConfig::getNotices(const string& channel, short verid, vector<Notice>& out_list)
{	
	out_list.clear();

	for (uint32_t i=0; i<notice_array_.size(); ++i )
	{
		Notice & curr_n = notice_array_[i];

		vector<string>& chans = curr_n.channal_list;

		vector<string>::iterator citer = find(chans.begin(), chans.end(),channel);

		if(citer != chans.end() || chans.empty())	
		{
			vector<int>& vers = curr_n.version;

			vector<int>::iterator viter = find(vers.begin(), vers.end(), verid);

			if(viter != vers.end() || vers.empty())
				out_list.push_back(curr_n);
		}
	}

	return 0;
}

