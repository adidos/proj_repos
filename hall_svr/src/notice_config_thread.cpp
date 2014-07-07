/* ======================================================
* 
* file:		notice_config_thread.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-11 16:05:27
* 
* ======================================================*/

#include "notice_config_thread.h"
#include "json/json.h"

#include "common/logger.h"

#include <unistd.h>
#include <string.h>

const static string NOTICE_KEY("notices");

NoticeConfigThread::NoticeConfigThread()
{
}

/**
* brief:
*
* @param redis_svr
* @param port
*
* @returns   
*/
int NoticeConfigThread::init(const string& redis_svr, short port)
{
	redis_handler_ptr_ = new RedisHandler(redis_svr, port);	
	return 0;
}

void NoticeConfigThread::doIt()
{
	vector<Notice> notice_array;
	NoticeConfig* ptr = NoticeConfig::getInstance();
	while(1)
	{		
		
		
		string config;
		int rst = redis_handler_ptr_->get_value(NOTICE_KEY, config);
		if(0 != rst)
		{
			LOG4CPLUS_ERROR(FLogger, "read notice config from redis failed, " 
				<< "result no : " << rst);
			
			sleep(60*5);
			continue;
		}
		
		notice_array.clear();
		rst = parseConfig(config, notice_array);
				
		LOG4CPLUS_DEBUG(FLogger, "Notice parseConfig " << notice_array.size());

		if ( notice_array.empty() )
		{
			ptr->clearAll();
			sleep(60*1);
			continue;
		}
		
		if(0 != rst)
		{
			LOG4CPLUS_ERROR(FLogger, "read notice config from redis failed, " 
				<< "result no : " << rst);
			
			sleep(60*5);
			continue;
		}

		CScopeGuard guard(mutex_);
		ptr->addNotice(notice_array);

		sleep(60*5);
	}

}

int NoticeConfigThread::parseConfig(const string& config, vector<Notice>& out_config)
{
	try 
	{
        Json::Reader reader;
        Json::Value root;
        if(!reader.parse(config, root, false)) 
		{
			LOG4CPLUS_ERROR(FLogger, "Json reader parse string " << config << "failed.");		
            return -1; 
        }   
   
        for(unsigned int i = 0; i < root.size(); ++i)
        {   
			Notice notice;

			string notice_id = root[i]["notice_id"].asString();
			string notice_type = root[i]["notice_type"].asString();
			string channal = root[i]["chn"].asString();
			
			char * pch = strtok((char *)channal.c_str(), ",");
		  	while (pch != NULL)
		  	{
			    notice.channal_list.push_back(pch);

			    pch = strtok (NULL, ",");
		  	}

			LOG4CPLUS_DEBUG(FLogger, "channal_list: "<< notice.channal_list.size());
			
			string vers = root[i]["ver"].asString();					

			if(! vers.empty())
			{
				pch = strtok((char*)vers.c_str(), ",");
				while(NULL != pch)
				{
					int  version = atoi(pch);
					notice.version.push_back(version);

					pch = strtok (NULL, ",");

					LOG4CPLUS_DEBUG(FLogger, "add a version: "<< version);
				}
			}
			
			LOG4CPLUS_DEBUG(FLogger, "version list size: "<< notice.version.size());
						
			notice.id = atoi(notice_id.c_str());	
			notice.type = atoi(notice_type.c_str());
			notice.title = root[i]["title"].asString();
			notice.content = root[i]["content"].asString();
			notice.begin_date = root[i]["begin_date"].asString();
			notice.end_date = root[i]["end_date"].asString();
			

			if(notice.isValid())
			{
				LOG4CPLUS_DEBUG(FLogger, 
						"{ id: " << notice.id << 
						", type : " << notice.type << 
						", title : " << notice.title << 
						", content : " << notice.content << 
						", ver:" << vers << 
						" channal: " << channal << 
						", begin date: " << notice.begin_date << 
						", end date: " << notice.end_date << 
						"}");

				out_config.push_back(notice);
			}
        }   
    }
	catch(exception& e)
	{ 
		LOG4CPLUS_ERROR(FLogger, "exception: " << e.what());         
    }

	return 0;
}

