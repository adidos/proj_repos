#ifndef PAYMENT_REQ_THREAD_H
#define PAYMENT_REQ_THREAD_H

#include <fstream>
#include <unistd.h>

#include "common/logger.h"
#include "common/thread.h"

class RedisHandler;
class Command;

class PaymentRequest
{
public:
	int type;
	int64_t uid;
	int product_id;
	string product_name;
	float price;
	int count;
		
	string dump() const {
		stringstream ss;
		ss << " uid: " << uid  << ", type:" << type
		   << ", product_id: " << product_id
		   << ", product_name: " << product_name
		   << ", price: " << price
		   << ", count: " << count;

		return ss.str();
	}
};

class PaymentResponse
{
public:	
	string device_id;
};

class PaymentReqHandler : public CThread
{
public:
	PaymentReqHandler(const string& redis_host,
		                       short redis_port);

	virtual ~PaymentReqHandler(){};

protected:
	virtual void doIt();

private:
	
	bool handleRequest(const PaymentRequest & payment_req);

	bool parse_str_payment_req(const string& str_payment_req, 
		                               PaymentRequest & payment_req);

	void handle_buy_bean(const PaymentRequest & payment_req);
	
	void handle_vip(const PaymentRequest &payment_req);

	void handle_rewards(const PaymentRequest &payment_req);

private:

	RedisHandler* m_redis;

	string 		  m_usersvr_ip;
	int           m_usersvr_port;

	ofstream      m_ofs;

};

#endif // PAYMENT_REQ_THREAD_H
