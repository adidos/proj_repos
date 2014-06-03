#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "configure.h"


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
	
};

#endif //APPLICATION_H
