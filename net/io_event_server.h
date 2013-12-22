#ifndef IO_EVENT_SERVER_H_
#define IO_EVENT_SERVER_H_

class IOEventServer
{
public:
	int addEvent() = 0;
	int delEvent() = 0;
	int modEvent() = 0;

	void start_server();

protected:



};

#endif /*IO_EVENT_SERVER_H_*/