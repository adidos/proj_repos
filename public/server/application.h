#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "common/configure.h"
#include "session_manager.h"
#include "servant.h"
#include "event_processor.h"
#include "worker_group.h"

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

	EventProcessor* _in_processor_ptr;

	WorkerGroup* _worker_group_ptr;
	
};

#endif //APPLICATION_H
