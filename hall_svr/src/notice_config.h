
/* ======================================================
* 
* file:		notice_config.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-11 15:50:37
* 
* ======================================================*/
#ifndef NOTICE_CONFIG_H
#define NOTICE_CONFIG_H

#include <string>
#include <vector>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

using namespace std;

struct Notice
{
	int id;
	int type;
	string title;
	string content;
	vector<int> version;
	vector<string> channal_list;
	string begin_date;
	string end_date;

	Notice(){};

	Notice(const Notice& other)
	{
		id = other.id;
		type = other.type;
		title = other.title;
		content = other.content;
		version = other.version;
		channal_list.assign(other.channal_list.begin(),
				other.channal_list.end());
		begin_date = other.begin_date;
		end_date = other.end_date;
	}

	Notice& operator =(const Notice& other)
	{
		id = other.id;
		type = other.type;
		title = other.title;
		content = other.content;
		version = other.version;
		channal_list.assign(other.channal_list.begin(),
				other.channal_list.end());
		begin_date = other.begin_date;
		end_date = other.end_date;

		return *this;
	}

	bool operator ==(const Notice& other) const
	{
		return (id == other.id && type == other.type);
	}

	bool operator !=(const Notice& other) const
	{
		return (id != other.id || type != other.type);
	}

	bool isValid() const
	{
		struct tm tf;
		struct timeval  tp;
		char buffer[32] = {'\0'};
		
		gettimeofday(&tp, NULL);
		localtime_r(&(tp.tv_sec), &tf);	
		sprintf(buffer,"%04d-%02d-%02d",tf.tm_year + 1900,
					tf.tm_mon + 1,tf.tm_mday);	
		
		string cur_date(buffer);
		if(cur_date.compare(begin_date) >= 0 &&
			cur_date.compare(end_date) <= 0)
			return true;

		return false;
	}
};

class NoticeConfig
{
public:
	NoticeConfig(){};

	int addNotice(Notice& notice);

	int addNotice(vector<Notice> notice_list, bool bAppend = false);

	int clearAll(){notice_array_.clear();return 0;};

	int getNotices(const string& channel, short verid,vector<Notice>& out_list);
	
	static NoticeConfig* getInstance()
	{
		if(NULL == instance_)
		{
			instance_ = new NoticeConfig();
		}
		return instance_;
	}


private:
	vector<Notice> notice_array_;

	static NoticeConfig * instance_;
};

#endif /*NOTICE_CONFIG_H*/
