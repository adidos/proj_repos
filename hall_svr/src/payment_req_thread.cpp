#include "payment_req_thread.h"

#include "redis_handler.h"
#include "json/json.h"

#include "common/utility.h"
#include "common/DataXCmd.h"
#include "common/XLDataX.h"
#include "common/data_id_const.h"
#include "common/configure.h"
#include "server/application.h"

#include "proxy_client.h"
#include "user_proxy.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <pthread.h>

using namespace std;

PaymentReqHandler::PaymentReqHandler(const string& redis_host,
	                                          short redis_port)
{
	m_redis = new RedisHandler(redis_host, redis_port);

	m_ofs.open("../log/payment_record.log");

}

void PaymentReqHandler::doIt()
{
	int empty_time = 0;
	LOG4CPLUS_DEBUG(FLogger, "PaymentReqHandler thread start");

	int iret = m_redis->subscribe_channel("payment");

	LOG4CPLUS_DEBUG(FLogger, "subscribe channel rst " << iret);

	while(true)
	{
		string str_payment;
		int rst = m_redis->get_subscribe_msg(str_payment);

    	// no msg
		if(1 == rst)
		{
			if(empty_time % 5000 == 0)
			{
				empty_time = 5000;
				LOG4CPLUS_DEBUG(FLogger, "the subscribe msg is empty.");
			}

			usleep(100*1000);
			empty_time++;
			continue;
		}
		// error
		else if(0 != rst)
		{
			LOG4CPLUS_ERROR(FLogger, "get_subscribe_msg error.");
			continue;
		}

		LOG4CPLUS_DEBUG(FLogger,  " begin handle_payment_req");
		if(!str_payment.empty())
		{
			PaymentRequest payment_req;
			bool parse_ok = parse_str_payment_req(str_payment, payment_req);
			if(parse_ok)
			{
				handleRequest(payment_req);	
			}			
		}

		LOG4CPLUS_DEBUG(FLogger, " end handle_payment_req");
	}
}

bool PaymentReqHandler::handleRequest(const PaymentRequest & payment_req)
{
	if(payment_req.type == 3004) //活动处理，目前活动只有金豆
	{
		handle_rewards(payment_req);	//活动奖励处理
	}
	else if(payment_req.type == 10) // vip
	{
		handle_vip(payment_req);
	}
	else if(payment_req.type == 0) //金豆
	{
		handle_buy_bean(payment_req);
	}
	
	return true;
}

void PaymentReqHandler::handle_vip(const PaymentRequest &payment_req)
{
	LOG4CPLUS_DEBUG(FLogger, "handle_vip " << payment_req.dump() );

	m_ofs << "handle vip begin send to usersvr : " 
		  <<  current_datetime() << payment_req.dump() << endl;

	UserProxy* proxy = (UserProxy*)(ProxyClient::Instance()->getServantPrxy("usersvr"));

	if(NULL == proxy)
	{
		m_ofs << "this reward handle failed!" <<endl;
		return ;
	}

	int64_t uid = payment_req.uid;
	int vip_level = payment_req.product_id % 1000; //vip 从1001 开始连续递增
	int valid_day = payment_req.count;
	
	VIPInfoResp vip_resp;
	int ret = proxy->updateVipInfo(uid, vip_level, valid_day, vip_resp);

	LOG4CPLUS_DEBUG(FLogger, "User buy " << vip_level << " vip , valid day "
		<< valid_day << ", call result " << ret << ", response: " << vip_resp.dump());

	//不论vip是否更新成功，购买vip的奖励还是要发放的
	UpdateGameInfoReq req;
	req.game_id = 1;
	req.bean_inc = 5000;
	
	UpdateGameInfoResp game_resp;
	ret = proxy->updateGameInfo(uid, req, game_resp);

	LOG4CPLUS_DEBUG(FLogger, "updateGameInfo return :" << ret << ", do request: "
			<< req.dump() << ", response: " << game_resp.dump()) ;

	//TODO notify user game information change 
		

	return;
}

void PaymentReqHandler::handle_rewards(const PaymentRequest &payment_req)
{
	LOG4CPLUS_DEBUG(FLogger, "handle_reward" << payment_req.dump() );

	m_ofs << "handle_reward begin send to usersvr : " 
		  <<  current_datetime() << payment_req.dump() << endl;

	UserProxy* proxy = (UserProxy*)(ProxyClient::Instance()->getServantPrxy("usersvr"));

	if(NULL == proxy)
	{
		m_ofs << "this reward handle failed!" <<endl;
		return ;
	}

	int uid = payment_req.uid;

	UpdateGameInfoReq req;
	req.game_id = 1;
	req.bean_inc = payment_req.count;

	UpdateGameInfoResp resp;
	int ret = proxy->updateGameInfo(payment_req.uid, req, resp);

	LOG4CPLUS_DEBUG(FLogger, "updateGameInfo return :" << ret << ", user " 
			<< uid << " do request: "<< req.dump() << ", response: " << resp.dump()) ;


	//TODO notify user game information change 
		
	return;
}

void PaymentReqHandler::handle_buy_bean(const PaymentRequest & payment_req)
{
	LOG4CPLUS_DEBUG(FLogger, "handle_payment_req " << payment_req.dump() );

	m_ofs << "handle_payment begin send to usersvr : " 
		  <<  current_datetime() << payment_req.dump() << endl;

	UserProxy* proxy = (UserProxy*)(ProxyClient::Instance()->getServantPrxy("usersvr"));

	if(NULL == proxy)
	{
		m_ofs << "this payment handle failed!" <<endl;
		return ;
	}

	int uid = payment_req.uid;
	
	UpdateGameInfoReq req;
	req.game_id = 1;

	//购买送双倍
	req.bean_inc = 2 * payment_req.count;

	UpdateGameInfoResp resp;
	int ret = proxy->updateGameInfo(payment_req.uid, req, resp);

	LOG4CPLUS_DEBUG(FLogger, "updateGameInfo return :" << ret << ", user " 
			<< uid << " do request: "<< req.dump() << ", response: " << resp.dump()) ;
	

	//TODO notify user game information change 
		
	return ;
}

bool PaymentReqHandler::parse_str_payment_req(const string& str_payment_req, 
		                                               PaymentRequest & payment_req)
{		
	LOG4CPLUS_DEBUG(FLogger, "parse_str_payment_req " << str_payment_req);
	try {
		Json::Reader reader;
		Json::Value root;
		
		if(!reader.parse(str_payment_req, root, false)) {
			LOG4CPLUS_ERROR(FLogger, "JSON parse " << str_payment_req << "failed." );
			return false; 
		} 

		payment_req.type = atoi(root["type"].asString().c_str());	

		istringstream iss(root["uid"].asString());	
		iss >> payment_req.uid;
	
		payment_req.product_id   = atoi(root["product_id"].asString().c_str());
		payment_req.product_name = root["product_name"].asString();
		payment_req.price = (float)atof(root["price"].asString().c_str());
		payment_req.count = atoi(root["count"].asString().c_str());
				
		LOG4CPLUS_DEBUG(FLogger, "Parse result: " << payment_req.dump());

	} catch(exception& e){
		LOG4CPLUS_DEBUG(FLogger, "Parse exception: " << e.what() );
		return false;
	}
	
	return true;
}



