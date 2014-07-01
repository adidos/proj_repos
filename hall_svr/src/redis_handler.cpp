/* ======================================================
* 
* file:		redis_handler.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-01-18 12:12:34
* 
* ======================================================*/

#include "redis_handler.h"
#include "common/logger.h"

#include <cassert>

RedisHandler::RedisHandler(string host, short port)
	: host_(host), port_(port), context_(NULL), queue_name_(""){
	conn2Redis();
}

RedisHandler::~RedisHandler() {
	if(NULL != context_) {
		redisFree(context_);
	}
}

/**
* brief:
*
* @param name
*/
void RedisHandler::queue_name(const string& name){
	queue_name_ = name;
} 

/**
* brief:
*
* @param value
*
* @returns   
*/
int RedisHandler::push_queue(const string& value) {
	return push_queue(queue_name_, value);	
}

/**
* brief:
*
* @param queue
* @param value
*
* @returns   
*/
int RedisHandler::push_queue(const string& queue, const string& value) {
	if(queue.empty() || value.empty()){
		LOG4CPLUS_ERROR(ALogger, "PUSH QUEUE INVALID PARAM");
		return -1;
	}
	redisReply* reply = (redisReply*)redisCommand(context_, "RPUSH %s %s", 
									queue.c_str(), value.c_str());

	if(REDIS_ERR_IO == context_->err){
		freeReply(reply);

		//如果是网络io错误，进行重新连接
		LOG4CPLUS_ERROR(ALogger, "net error occur, err: " << context_->errstr << ", reconnect...");
		int rst = conn2Redis();
		if(0 != rst) {
			LOG4CPLUS_ERROR(ALogger, "Reconnect server failed, err: " << context_->errstr);
			return -1;	
		}

		reply = (redisReply*)redisCommand(context_, "RPUSH %s %s",queue.c_str(), value.c_str());
	} else if(REDIS_OK != context_->err) {
		freeReply(reply);
		LOG4CPLUS_ERROR(ALogger, "RPUSH: failed, err: " << context_->errstr);
		return context_->err;
	}

	LOG4CPLUS_DEBUG(ALogger, "RPUSH: push " << value << " into queue[" << queue 
					<< "], info: " << context_->errstr);

	freeReply(reply);
	return context_->err;	
}

/**
* brief:
*
* @param value
*
* @returns   
*/
int RedisHandler::pop_queue(string& value) {
	return pop_queue(queue_name_, value);
}

/**
* brief:
*
* @param queue
* @param value
*
* @returns   
*/
int RedisHandler::pop_queue(const string& queue, string& value) {
	if(queue.empty()) {
		LOG4CPLUS_ERROR(ALogger, "POP QUEUE INVALID PARAM");
		return -1;
	}

	redisReply* reply = (redisReply*)redisCommand(context_, "LPOP %s",queue.c_str());
	if(REDIS_ERR_IO == context_->err){
		freeReply(reply);

		//如果是网络io错误，进行重新连接
		LOG4CPLUS_ERROR(ALogger, "net error occur, err: " << context_->errstr << ", reconnect...");
		int rst = conn2Redis();
		if(0 != rst) {
			LOG4CPLUS_ERROR(ALogger, "Reconnect server failed, err: " << context_->errstr);
			return -1;	
		}

		reply = (redisReply*)redisCommand(context_, "LPOP %s",queue.c_str());
	} else if(REDIS_OK != context_->err) {
		LOG4CPLUS_ERROR(ALogger, "POP : failed, err: " << context_->errstr);
		freeReply(reply);
		return context_->err;
	}

	if(REDIS_OK == context_->err && NULL != reply) {
		if(REDIS_REPLY_STRING == reply->type) {
			value.assign(reply->str, reply->len);
		}else if(REDIS_REPLY_ARRAY == reply->type) {
			for(unsigned int i = 0; i < reply->elements; ++i) {
				value.assign(reply->element[i]->str, reply->element[i]->len);
			}
		}else if(REDIS_REPLY_NIL == reply->type) {
			return 1; //队列为空		
		}
	}
		
	LOG4CPLUS_DEBUG(ALogger, "POP: pop " << value << " from queue[" << queue << "]");
	
	freeReply(reply);
	return context_->err;
}

int RedisHandler::get_value(const string& key, string& value)
{
	if(key.empty()) {
		LOG4CPLUS_ERROR(ALogger, "GET VALUE INVALID PARAM");
		return -1;
	}

	redisReply* reply = (redisReply*)redisCommand(context_, "GET %s",key.c_str());
	if(REDIS_ERR_IO == context_->err){
		freeReply(reply);

		//如果是网络io错误，进行重新连接
		LOG4CPLUS_ERROR(ALogger, "net error occur, err: " << context_->errstr << ", reconnect...");
		int rst = conn2Redis();
		if(0 != rst) {
			LOG4CPLUS_ERROR(ALogger, "Reconnect server failed, err: " << context_->errstr);
			return -1;	
		}

		reply = (redisReply*)redisCommand(context_, "GET %s",key.c_str());
	} else if(REDIS_OK != context_->err) {
		LOG4CPLUS_ERROR(ALogger, "GET : failed, err: " << context_->errstr);
		freeReply(reply);
		return context_->err;
	}

	if(REDIS_OK == context_->err && NULL != reply) {
		if(REDIS_REPLY_STRING == reply->type) {
			value.assign(reply->str, reply->len);
		}else if(REDIS_REPLY_ARRAY == reply->type) {
			//TODO	
		}else if(REDIS_REPLY_NIL == reply->type) {
		}
	}
		
	LOG4CPLUS_DEBUG(ALogger, "Get: get " << key << ", value " << value << "]");
	
	freeReply(reply);
	return context_->err;
}

int RedisHandler::set_value(const string& key, const string& value){
	if(key.empty() || value.empty()) return -1;

	redisReply* reply = (redisReply*)redisCommand(context_, "SET %s %s EX %d",
				key.c_str(), value.c_str(), REDIS_VALUE_TIMEOUT);
	if(REDIS_ERR_IO == context_->err){
		freeReply(reply);

		//如果是网络io错误，进行重新连接
		LOG4CPLUS_ERROR(ALogger, "net error occur, err: " << context_->errstr << ", reconnect...");
		int rst = conn2Redis();
		if(0 != rst) {
			LOG4CPLUS_ERROR(ALogger, "Reconnect server failed, err: " << context_->errstr);
			return -1;	
		}

		reply = (redisReply*)redisCommand(context_, "SET %s %s EX %d",
				key.c_str(), value.c_str(), REDIS_VALUE_TIMEOUT);
	} else if(REDIS_OK != context_->err) {
		LOG4CPLUS_ERROR(ALogger, "SET: failed, err: " << context_->errstr);
		freeReply(reply);
		return context_->err;
	}
		
	LOG4CPLUS_DEBUG(ALogger, "SET: set " << key << " \t value " << value );
	
	freeReply(reply);
	return context_->err;

	return 0;
}

int RedisHandler::conn2Redis() {
	assert(port_ > 0 && port_ < 65535);

	if(host_.empty())
		host_ = string("127.0.0.1");

	if(NULL != context_) {
		redisFree(context_);
	}

	struct timeval timeout = {1, 500000};	//连接1.5秒超时
	
	context_ = redisConnectWithTimeout(host_.c_str(), port_, timeout);
	if(NULL == context_ || context_->err)
	{
		if(NULL != context_)
		{
			LOG4CPLUS_ERROR(ALogger, "connect to redis server " << host_ << ":"
				<< port_ << " error, msg: " << context_->errstr);
			redisFree(context_);
			return -1;
		}

		LOG4CPLUS_ERROR(ALogger, "connect to redis server " << host_ << ":"
				<< port_ << " error, system error!");
		return -1;
	}

	return 0;
}

int RedisHandler::subscribe_channel( const string & channel_name )
{	
	redisReply* reply = (redisReply*)redisCommand(context_, "SUBSCRIBE %s", channel_name.c_str());
	if(REDIS_ERR_IO == context_->err){
		freeReply(reply);

		//如果是网络io错误，进行重新连接
		LOG4CPLUS_ERROR(ALogger, "net error occur, err: " << context_->errstr << ", reconnect...");
		int rst = conn2Redis();
		if(0 != rst) {
			LOG4CPLUS_ERROR(ALogger, "Reconnect server failed, err: " << context_->errstr);
			return -1;	
		}

		reply = (redisReply*)redisCommand(context_, "SUBSCRIBE %s", channel_name.c_str());
	} else if(REDIS_OK != context_->err) {
		LOG4CPLUS_ERROR(ALogger, "subscribe : failed, err: " << context_->errstr);
		freeReply(reply);
		return context_->err;
	}				
	
	freeReply(reply);
	return context_->err;
}

int RedisHandler::unsubscribe_channel( const string & channel_name )
{
	if(channel_name.empty()){
		LOG4CPLUS_ERROR(ALogger, "unsubscribe_channel INVALID PARAM");
		return -1;
	}

	redisReply* reply = (redisReply*)redisCommand(context_, "UNSUBSCRIBE %s", channel_name.c_str());
	if(REDIS_ERR_IO == context_->err){
		freeReply(reply);

		//如果是网络io错误，进行重新连接
		LOG4CPLUS_ERROR(ALogger, "net error occur, err: " << context_->errstr << ", reconnect...");
		int rst = conn2Redis();
		if(0 != rst) {
			LOG4CPLUS_ERROR(ALogger, "Reconnect server failed, err: " << context_->errstr);
			return -1;	
		}

		reply = (redisReply*)redisCommand(context_, "UNSUBSCRIBE %s", channel_name.c_str());
	} else if(REDIS_OK != context_->err) {
		LOG4CPLUS_ERROR(ALogger, "subscribe : failed, err: " << context_->errstr);
		freeReply(reply);
		return context_->err;
	}
	
	freeReply(reply);
	return context_->err;
}


int RedisHandler::get_subscribe_msg(string & str_msg)
{
	redisReply *reply;
	int iret = redisGetReply(context_, (void **)&reply);

	if(REDIS_OK == iret && NULL != reply) {
		if(REDIS_REPLY_STRING == reply->type) {
			str_msg.assign(reply->str, reply->len);
		}else if(REDIS_REPLY_ARRAY == reply->type) {
			for(unsigned int i = 0; i < reply->elements; ++i) {
				str_msg.assign(reply->element[i]->str, reply->element[i]->len);
			}
		}else if(REDIS_REPLY_NIL == reply->type) {
			return 1; //队列为空		
		}
	}
	
	return iret;
}
