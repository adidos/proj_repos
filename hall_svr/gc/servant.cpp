
/* ======================================================
* 
* file:		servant.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-28 10:54:37
* 
* ======================================================*/

int	 Servant::init()
{

}

int  Servant::startService()
{

}

bool Servant::insertRecvQueue(DataXCmd* pCmd)
{
	int wait_ms = 50;
	return _recv_queue.push(pCmd, wait_ms);
}

bool Servant::insertSendQueue(DataXCmd* pCmd)
{
	int wait_ms = 50;
	return _send_queue.push(pCmd, wait_ms);
}

bool Servant::getRecvCommand(DataXCmd* pCmd)
{
	int wait_ms = 50;
	return _recv_queue.pop(pCmd, wait_ms);
}

bool Servant::getSendCommand(DataXCmd* pCmd)
{
	int wait_ms = 50;
	return _send_queue.pop(pCmd, wait_ms);
}

