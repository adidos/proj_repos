/* ======================================================
* 
* file:		redis_handler.h
* brief:	封装redis client api接口
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-01-18 11:55:31
* 
* ======================================================*/

#ifndef REDIS_HANDLER_H_
#define REDIS_HANDLER_H_

#include <string>

#include "hiredis.h"

using namespace std;

#define REDIS_VALUE_TIMEOUT 60

class RedisHandler 
{
public:
	RedisHandler(string host = "127.0.0.1", short port = 6379);
	~RedisHandler();

	void queue_name(const string& name);

	int push_queue(const string& value);

	int push_queue(const string& queue, const string& value);

	int pop_queue(string& value);

	int pop_queue(const string& queue, string& value);

	int set_value(const string& key, const string& value);

	int get_value(const string& key, string& value);

	int subscribe_channel( const string & channel_name );

	int unsubscribe_channel( const string & channel_name );

	int get_subscribe_msg(string & msg);
	
private:
	int conn2Redis();
	
	void freeReply(redisReply* reply)
	{
		if(NULL != reply)
		{
			delete reply;
			reply = NULL;
		}
	}

private:
	string	host_;
	short	port_;

	redisContext* context_;
	
	string queue_name_;
};
#endif /*REDIS_HANDLER_H_*/

