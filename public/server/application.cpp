#include "application.h"

Configure* g_pConfig = NULL;

Application::Application()
	: _config_ptr(NULL), _sess_mgr_ptr(NULL),
	_servant_ptr(NULL), _in_processor_ptr(NULL)
{
}

Application::~Application()
{
	if(NULL != _config_ptr)
	{
		delete _config_ptr;
		_config_ptr = NULL;
	}

	if(NULL != _sess_mgr_ptr)
	{
		delete _sess_mgr_ptr;
		_sess_mgr_ptr = NULL;
	}

	if(NULL != _servant_ptr)
	{
		delete _servant_ptr;
		_servant_ptr = NULL;
	}

	if(NULL != _in_processor_ptr)
	{
		delete _in_processor_ptr;
		_in_processor_ptr = NULL;
	}
	
}

void Application::initialize(const string & file)
{
	if(NULL != _config_ptr)
	{
		delete _config_ptr;
		_config_ptr = NULL;
	}

	_config_ptr = new Configure(file);
	g_pConfig = _config_ptr;
	
	string log_conf_path = _config_ptr->getString("log.config.path");
	CLogger::init(log_conf_path);
	CDebugLogger::init(log_conf_path);

	_sess_mgr_ptr = new SessionManager();
	
	_servant_ptr = new Servant(_sess_mgr_ptr);
	_servant_ptr->init();
	
	_in_processor_ptr = new EventProcessor(_sess_mgr_ptr);
	_in_processor_ptr.regEventServer(_servant_ptr->getEpollServer());

	_servant_ptr->getEpollServer()->regProcessor(_in_processor_ptr);		
}

int Application::waitForShutdown()
{
	_in_processor_ptr->start();

	_servant_ptr->startService();

	_in_processor_ptr->waitForStop();
	
	return 0;
}
