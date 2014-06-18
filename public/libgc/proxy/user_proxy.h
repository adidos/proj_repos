
/* ======================================================
* 
* file:		user_proxy.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-18 11:25:29
* 
* ======================================================*/

#ifndef USER_PROXY_H
#define USER_PROXY_H

#include <stdint.h>

#include <map>
#include <string>

#include "servant_proxy.h"

using namespace std;
//class GameInfo;

class UserProxy : public ServantProxy
{
public:
	UserProxy();
	~UserProxy();

	int updateUserBasic(const map<short, string>& infos, int& result);

	//int updateGameInfo(const GameInfo& info);

	int updateVipInfo(int64_t uid, char vip_level, int vaild_day, int& result);

	//int getUserInfo();
	
	int checkin(int64_t uid, int& result);
};

#endif //USER_PROXY_H
