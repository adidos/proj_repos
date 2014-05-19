#ifndef NOTIFY_INFO_H
#define NOTIFY_INFO_H

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
};

#endif //NOTIFY_INFO_H
