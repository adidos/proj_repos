#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "common/configure.h"


class SessionManager;
class Servant;
class EventProcessor;
class WorkerGroup;
class RespProcessor;

using namespace std;

class Application
{
public:
	Application();
	~Application();

	void initialize(const string &file);

	int waitForShutdown();

	Configure* getConfigure()
	{
		return _config_ptr;
	}

private:
	Configure* _config_ptr;

	SessionManager* _sess_mgr_ptr;

	Servant* _servant_ptr;

	EventProcessor* _req_processor_ptr;

	WorkerGroup* _worker_group_ptr;

	RespProcessor* _resp_processor_ptr;
};

#endif //APPLICATION_H
