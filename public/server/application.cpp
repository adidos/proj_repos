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

	//init configure
	_config_ptr = new Configure(file);
	g_pConfig = _config_ptr;

	//init logger
	string log_conf_path = _config_ptr->getString("log.config.path");
	CLogger::init(log_conf_path);
	CDebugLogger::init(log_conf_path);

	//worker group
	_worker_group_ptr = new WorkerGroup();
	int work_num = _config_ptr->getInt("worker.number", 1);
	_worker_group_ptr->init(work_num);
	
	_sess_mgr_ptr = new SessionManager();
	
	_servant_ptr = new Servant(_sess_mgr_ptr);
	_servant_ptr->init();
	
	_in_processor_ptr = new EventProcessor(_sess_mgr_ptr, _worker_group_ptr);
	_in_processor_ptr.regEventServer(_servant_ptr->getEpollServer());

	_servant_ptr->getEpollServer()->regProcessor(_in_processor_ptr);
}

int Application::waitForShutdown()
{
	_worker_group_ptr->startWorker();
	_in_processor_ptr->start();
	_servant_ptr->startService();

	_servant_ptr->waitForStop();
	_in_processor_ptr->waitForStop();
	_worker_group_ptr->waitForStop();
	
	return 0;
}
