#include "notice_config_thread.h"
#include "Utility.h"
#include "json/json.h"

const static string NOTICE_KEY("notices");

IMPL_LOGGER(NoticeConfigThread, logger);

NoticeConfigThread::NoticeConfigThread(const string& redis_svr, short port)
{
	init(redis_svr, port);
}

int NoticeConfigThread::init(const string& redis_svr, short port)
{
	redis_handler_ptr_ = new RedisHandler(redis_svr, port);	
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
			LOG4CPLUS_ERROR(logger, "read notice config from redis failed, " 
				<< "result no : " << rst);
			
			//Utility::sleep_in_seconds(60*5);	
			sleep(60*5);
			continue;
		}
		
		notice_array.clear();
		rst = parseConfig(config, notice_array);
				
		LOG4CPLUS_DEBUG(logger, "Notice parseConfig " << notice_array.size());

		if ( notice_array.empty() )
		{
			ptr->clearAll();
			sleep(60*1);
			continue;
		}
		
		if(0 != rst)
		{
			LOG4CPLUS_ERROR(logger, "read notice config from redis failed, " 
				<< "result no : " << rst);
			
			//Utility::sleep_in_seconds(60*5);	
			sleep(60*5);
			continue;
		}

		CScopeGuard guard(mutex_);
		ptr->addNotice(notice_array);

		Utility::sleep_in_seconds(60*5);	
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
			LOG4CPLUS_ERROR(logger, "Json reader parse string " << config << "failed.");		
            return -1; 
        }   
   
        for(int i = 0; i < root.size(); ++i)
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

			LOG4CPLUS_DEBUG(logger, "channal_list: "<< notice.channal_list.size());
			
			for ( int i=0; i<notice.channal_list.size(); ++i )
			{
				//LOG4CPLUS_DEBUG(logger, ""<< i << ": " << notice.channal_list[i]);
			}

			
			notice.id = atoi(notice_id.c_str());	
			notice.type = atoi(notice_type.c_str());
			notice.title = root[i]["title"].asString();
			notice.content = root[i]["content"].asString();
			notice.version = atoi(root[i]["ver"].asString().c_str());					
			notice.begin_date = root[i]["begin_date"].asString();
			notice.end_date = root[i]["end_date"].asString();
			

			if(notice.isValid())
			{
				LOG4CPLUS_DEBUG(logger, 
						"{ id: " << notice.id << 
						", type : " << notice.type << 
						", title : " << notice.title << 
						", content : " << notice.content << 
						", ver:" << notice.version << 
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
		LOG4CPLUS_ERROR(logger, "exception: " << e.what());         
    }

	return 0;
}

