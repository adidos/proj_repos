#include "payment_req_thread.h"

#include "redis_handler.h"
#include "json/json.h"

#include "common/DataXCmd.h"
#include "common/XLDataX.h"
#include "common/data_id_const.h"
#include "common/configure.h"
#include "server/application.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <pthread.h>

using namespace std;

extern Application* g_pApp;

PaymentReqHandler::PaymentReqHandler(const string& redis_host,
	                                          short redis_port)
{
	Configure* pConfig = g_pApp->getConfigure();
	
	m_usersvr_ip   = pConfig->getString("usersvr_ip", "", false);	
	m_usersvr_port = pConfig->getInt("usersvr_port", 0, false);

	
	m_redis = new RedisHandler(redis_host, redis_port);
	int iret = m_redis->subscribe_channel("payment");

	m_ofs.open("../log/payment_record.log");

	LOG4CPLUS_DEBUG(CLogger::logger, "subscribe channel rst " << iret);
}

void PaymentReqHandler::doIt()
{
	int empty_time = 0;
	LOG4CPLUS_DEBUG(CLogger::logger, "PaymentReqHandler thread start");

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
				LOG4CPLUS_DEBUG(CLogger::logger, "the subscribe msg is empty.");
			}

			usleep(100*1000);
			empty_time++;
			continue;
		}
		// error
		else if(0 != rst)
		{
			LOG4CPLUS_ERROR(CLogger::logger, "get_subscribe_msg error.");
			continue;
		}

		LOG4CPLUS_DEBUG(CLogger::logger,  " begin handle_payment_req");
		if(!str_payment.empty())
		{
			PaymentRequest payment_req;
			bool parse_ok = parse_str_payment_req(str_payment, payment_req);
			if(parse_ok)
			{
				handle_payment_req(payment_req);	
			}			
		}

		LOG4CPLUS_DEBUG(CLogger::logger, " end handle_payment_req");
	}
}

bool PaymentReqHandler::handleRequest(const PaymentRequest & payment_req)
{
	if(payment_req.type == "3004") //活动处理，目前活动只有金豆
		//reward_handle(paymen_req);	//活动奖励处理
	else
	{
		if(payment_req.product_id == "")
			handle_vip(payment_req);
		else
			handle_payment_req(payment_req);

	}
}

bool PaymentReqHandler::parse_str_payment_req(const string& str_payment_req, 
		                                               PaymentRequest & payment_req)
{		
	LOG4CPLUS_DEBUG(CLogger::logger, "parse_str_payment_req " << str_payment_req);
	try {
		Json::Reader reader;
		Json::Value root;
		
		if(!reader.parse(str_payment_req, root, false)) {
			LOG4CPLUS_ERROR(CLogger::logger, "JSON parse " << str_payment_req << "failed." );
			return false; 
		}   

		paymen_req.type			 = roo["type"].asString();
		payment_req.uid          = root["uid"].asString();
		payment_req.product_id   = root["product_id"].asString();
		payment_req.product_name = root["product_name"].asString();
		payment_req.price        = root["price"].asString();
		payment_req.count        = root["count"].asString();
				
		LOG4CPLUS_DEBUG(CLogger::logger, "Parse result: " << payment_req.dump());

	} catch(exception& e){
		LOG4CPLUS_DEBUG(CLogger::logger, "Parse exception: " << e.what() );
		return false;
	}
	
	return true;
}

void PaymentReqHandler::handle_payment_req(const PaymentRequest & payment_req)
{
	LOG4CPLUS_DEBUG(CLogger::logger, "handle_payment_req " << payment_req.dump() );

	m_ofs << "handle_payment begin send to usersvr : " 
		  << Utility::get_cur_4y2m2d2h2m2s() 
		  << payment_req.dump() << endl;
	
	Command * req_cmd = create_UpdateGameInfo(payment_req);

	// send to UserSvr
	AutoByteBuf<1024> auto_buf;
	int expected_len = req_cmd->encoded_length();
	byte * send_buf = auto_buf.get_buffer(expected_len);
	bool succ = req_cmd->encode(send_buf, expected_len);
	delete req_cmd;

	if ( !succ )
	{
		LOG4CPLUS_ERROR(CLogger::logger, "encode failed.");
		m_ofs << "handle_payment begin send to usersvr: encode err" << endl;
        return ;
	}

	int ret;
    struct sockaddr_in addr = {0};
    struct in_addr x_addr;
	
    inet_aton(m_usersvr_ip.c_str(), &x_addr);	
    addr.sin_family = AF_INET;
    addr.sin_addr   = x_addr;
    addr.sin_port   = htons(m_usersvr_port);
	
    int fd = socket(AF_INET,SOCK_STREAM, 0);
	
    if ( fd == -1 )
    {
    	LOG4CPLUS_ERROR(CLogger::logger, "create socket error: " << strerror(errno));
        return ;
    }
	
    ret = connect(fd, (sockaddr*)&addr, sizeof(addr));
	
    if ( ret == -1 )
    {
    	LOG4CPLUS_ERROR(CLogger::logger, "connect error: " << strerror(errno));
        return ;
    }

	m_ofs << "handle_payment begin send to usersvr" << endl;
	ret = send(fd, send_buf, expected_len, 0);
	if ( ret == -1 )
	{
		LOG4CPLUS_ERROR(CLogger::logger, "send data error: " << strerror(errno));
        return ;
	}
	else
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "send ret " << ret);
	}

	////////////////////////

	char recv_buf[4096];
    ret = recv(fd, recv_buf, sizeof(recv_buf), 0);
	m_ofs << "handle_payment afer recv from usersvr: ready decode" << endl;
    if ( ret == -1 )
    {
    	LOG4CPLUS_ERROR(CLogger::logger, "receive resp error ");
        return ;
    }
	else
	{
		LOG4CPLUS_DEBUG(CLogger::logger, "recv " << ret);
	}

	DataXCmd rst_cmd;
	if ( !rst_cmd.decode_header((byte*)recv_buf, ret) )
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode resp header failed!" );        
        return ;
	}
	if ( !rst_cmd.decode_parameters((byte*)recv_buf + rst_cmd.header_length(), 
										ret - rst_cmd.header_length()) )
	{
		LOG4CPLUS_ERROR(CLogger::logger, "decode resp body failed!");        
        return ;
	}

	LOG4CPLUS_DEBUG(CLogger::logger, "resp datax cmd name " << rst_cmd.get_cmd_name());	

	m_ofs << "handle_payment afer recv from usersvr: succ decode" << endl;
	
	IDataX * dx2 = rst_cmd.get_datax();
	
	if ( dx2 != NULL ) 
	{
		DxWrapper wrapper(dx2);
		
		LOG4CPLUS_DEBUG(CLogger::logger, "DataID_Result:  " << wrapper.GetInt(DataID_Result) );

		m_ofs << "handle_payment afer recv from usersvr: UpdateGameInfoResp " 
              << " Result = "  << wrapper.GetInt(DataID_Result)
              << " GameId = "  << wrapper.GetInt(DataID_GameId)
              << " WinNum = "  << wrapper.GetInt(DataID_WinNum)
              << " LoseNum = " << wrapper.GetInt(DataID_LoseNum)
              << " BeanNum = " << wrapper.GetInt(DataID_BeanNum)
		      << endl;
						
		IDataX * dx = new XLDataX();

		dx->PutInt(DataID_GameId, wrapper.GetInt(DataID_GameId));
		dx->PutInt(DataID_BeanNum, wrapper.GetInt(DataID_BeanNum));
		dx->PutInt(DataID_WinNum, wrapper.GetInt(DataID_WinNum));
		dx->PutInt(DataID_LoseNum, wrapper.GetInt(DataID_LoseNum));

		DataXCmd * delay_rst_cmd = new DataXCmd("GameInfoChangeNotify", CIPHER_MODE_TEA);

		uint64_t num_userid = Utility::string_to_uint64(payment_req.uid);
		
		delay_rst_cmd->set_userid(num_userid);
		delay_rst_cmd->set_datax(dx);

		delay_rst_msg drst_msg(num_userid, delay_rst_cmd);

		LOG4CPLUS_DEBUG(CLogger::logger, "put payment userid " << num_userid);	
		
		delete delay_rst_cmd;
		
	}
		
	return ;
}

Command * PaymentReqHandler::create_UpdateGameInfo(const PaymentRequest & payment_req)
{
	IDataX * dx = new XLDataX();

	int bean_cnt = atoi(payment_req.count.c_str());
	bean_cnt *= 2;
		
	dx->PutInt(DataID_GameId, 1);
	dx->PutInt(DataID_WinInc, 0);
	dx->PutInt(DataID_LoseInc, 0);
	dx->PutInt(DataID_BeanInc, bean_cnt);// ToDo
	
	DataXCmd * dx_cmd = new DataXCmd("UpdateGameInfo", CIPHER_MODE_TEA);	

	__int64 num_userid = Utility::string_to_uint64(payment_req.uid);
	
	dx_cmd->set_userid(num_userid);
	LOG4CPLUS_DEBUG(CLogger::logger, "create_UpdateGameInfo userId = " << num_userid);	
	
	dx_cmd->set_datax(dx); 

	m_ofs << "handle_payment begin send to usersvr : " 
		  << " userid = " << dx_cmd->get_userid()
		  << " beaninc = " << bean_cnt
		  << endl;

	return dx_cmd;
}


