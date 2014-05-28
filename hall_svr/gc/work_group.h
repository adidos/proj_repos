
/* ======================================================
* 
* file:		work_group.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-28 18:28:35
* 
* ======================================================*/

#ifndef WORK_GROUP_H
#define WORK_GROUP_H

#include "servant.h"

class CmdHandler;

class CWorkerGroup : public CThread
{
public:
	CWorkerGroup(Servant* pServant);
	~CWorkerGroup();
	
	void regCmdHandler(const string& name, CmdHandler* pHandler)
	{
		_handler_array[name] = pHandler;
	}

	void notifyUserDrop(int seqno);

protected:
	virtual void doIt();

private:
	Servant* _servant_ptr;
	bool _terminate;

	UserSession* _user_session;

	map<string, CmdHandler*> _handler_array;
};

#endif //WORK_GROUP_H
