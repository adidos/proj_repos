
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

//class GameInfo;

class UserProxy : public ServantProxy
{
public:
	UserProxy();
	~UserProxy();

	int updateUserBasic(const map<short, string>& infos);

	//int updateGameInfo(const GameInfo& info);

	int updateVipInfo(int vip_level, int vaild_day);

	//int getUserInfo();
	
	int checkin();
};

#endif //USER_PROXY_H
