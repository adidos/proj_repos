#include "notice_config.h"
#include <algorithm>

IMPL_LOGGER(NoticeConfig, logger);

NoticeConfig * NoticeConfig::instance_ = NULL;


int NoticeConfig::addNotice(Notice& notice)
{

	vector<Notice>::iterator iter = find(notice_array_.begin(),
			notice_array_.end(), notice);

	if(iter == notice_array_.end())
	{
		LOG4CPLUS_DEBUG(logger, "addNotice()");
		notice_array_.push_back(notice);
	}

	return 0;
}

int NoticeConfig::addNotice(vector<Notice> notice_list, bool bAppend)
{
	LOG4CPLUS_DEBUG(logger, "addNotice list " << notice_list.size());
	
	if ( !bAppend )
		notice_array_.clear();

	for (int i=0; i<notice_list.size(); ++i)
	{
		notice_array_.push_back(notice_list[i]);
	}
	
	return 0;
}


int NoticeConfig::getNotices(const string& channel, short verid, vector<Notice>& out_list)
{	
	out_list.clear();

	for ( int i=0; i<notice_array_.size(); ++i )
	{
		Notice & curr_n = notice_array_[i];

		if ( curr_n.version == verid  )
		{
			vector<string> & channal_list = curr_n.channal_list;
			for ( int j=0; j<channal_list.size(); ++j )
			{
				if ( channal_list[j] == channel )
				{
					out_list.push_back(curr_n);
					break;
				}
			}
		}
	}

	return 0;
}

