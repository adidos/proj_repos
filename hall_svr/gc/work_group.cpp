
/* ======================================================
* 
* file:		work_group.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-28 19:15:52
* 
* ======================================================*/

#include "work_group.h"
#include ""

void CWorkerGroup::notifyUserDrop(int seqno)
{


}


void CWorkerGroup::doIt()
{
	while(!_terminate)
	{
		DataXCmd* pCmd = NULL;	
		bool bSucc = _servant_ptr->getRecvCommand(pCmd);
		if(!bSucc)
		{
			LOG4CPLUS_ERROR(CLogger::logger, "get command from servant receive queue failed.");
			continue;
		}

		uint64_t uid = pCmd->get_userid();

	}

}
