
/* ======================================================
* 
* file:		notify_info.h
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-05-20 14:26:49
* 
* ======================================================*/
#ifndef NOTIFY_INFO_H
#define NOTIFY_INFO_H

enum NotifyType
{
	NT_CLOSE,
	NT_READ,
	NT_WRITE
};

struct NotifyInfo
{
	int seqno;
	int type;
	
	NotifyInfo()
	{
		seqno = 0;
		type = -1;
	}

	NotifyInfo(const NotifyInfo& other)
	{
		seqno = other.seqno;
		type = other.type;
	}

	NotifyInfo& operator=(const NotifyInfo& other)
	{
		seqno = other.seqno;
		type = other.type;
		return *this;
	}

	bool isValid()
	{
		if(seqno < 0)
			return false;
		if(type > NT_WRITE || type < NT_CLOSE)
			return false;
		
		return true;
	}

	string dump()
	{
		ostringtream oss;
		oss << "{" << seqno << ", " << type << "}";
		
		return oss.str();
	}
};

#endif //NOTIFY_INFO_H
