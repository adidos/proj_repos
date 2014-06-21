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
	string type;
	string uid;
	string product_id;
	string product_name;
	string price;
	string count;
		
	string dump() const {
		stringstream ss;
		ss << " uid: " << uid 
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

	bool parse_str_payment_req(const string& str_payment_req, 
		                               PaymentRequest & payment_req);

	void handle_payment_req(const PaymentRequest & payment_req);

	Command * create_UpdateGameInfo(const PaymentRequest & payment_req);

private:

	RedisHandler* m_redis;

	string 		  m_usersvr_ip;
	int           m_usersvr_port;

	ofstream      m_ofs;

};

#endif // PAYMENT_REQ_THREAD_H
