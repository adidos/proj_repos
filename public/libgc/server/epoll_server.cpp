
/* ======================================================
 * 
 * file:		_epollserver.cpp
 * brief:	
 * author:	80070525(chenjian)
 * version:	1.0.0
 * date:		2014-05-04 19:53:03
 * 
 * ======================================================*/

#include "epoll_server.h"
#include "session_manager.h"
#include "event.h"
#include "event_processor.h"
#include "common/logger.h"
#include "common/utility.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include <utility>
#include <functional>

using namespace std;

EpollServer::EpollServer()
:_event_processor_ptr(NULL)
{
}

/**
 * brief:
 *
 * @param size
 *
 * @returns   
 */
int EpollServer::init(int size)
{
	_epoll.create(size);

	return 0;
}

int EpollServer::notify(int fd, uint64_t data, int iEvent)
{
	LOG4CPLUS_TRACE(FLogger, "epoll notify: " << fd << "|" 
				<< data<< "|" << iEvent );
	
	CScopeGuard guard(_mutex);

	switch(iEvent)
	{
		case EVENT_NEW:
			_epoll.del(fd, 0, 0);
			_epoll.add(fd, data, EPOLLET | EPOLLIN);	
			_fd_array[data] = time(NULL);
			break;

		case EVENT_READ:
			_epoll.mod(fd, data, EPOLLET | EPOLLIN);
			break;

		case EVENT_WRITE:
			_epoll.mod(fd, data, EPOLLET | EPOLLOUT);
			break;

		case EVENT_CLOSE:
		case EVENT_ERROR:
			_epoll.del(fd, data, EPOLLET | EPOLLIN | EPOLLOUT);
			break;

		default:
			LOG4CPLUS_ERROR(FLogger, "notify unknow event, " << iEvent);
	}

	return 0;
}

/**
 * brief:
 *
 * @returns   
 */
void EpollServer::doIt()
{
	LOG4CPLUS_DEBUG(FLogger, "event processor start work!");

	time_t last_check = time(NULL);
	while(!_terminate)
	{
		//block until event occure
		int ret = _epoll.wait(EPOLL_WAIT_TIME);
		if(ret < 0)
		{
			if(errno == EINTR)
			  continue;

			LOG4CPLUS_ERROR(FLogger, "epoll server error, exit run loop...");
			break;
		}

		LOG4CPLUS_TRACE(FLogger, "epoll wait return value " << ret);
		for(int i = 0; i < ret; ++i)
		{
			struct epoll_event ev = _epoll.get(i);

			Event event = {'\0'};
			event.seqno = H32(ev.data.u64);
			event.timestamp = current_time_usec();
			if(ev.events & (EPOLLHUP | EPOLLERR))
			{
				LOG4CPLUS_TRACE(FLogger, "epoll error, fd = " << ev.data.fd
							<< ", seqno = " << H32(ev.data.u64));
				event.type = EVENT_CLOSE;
				
				_epoll.del(ev.data.fd, ev.data.u64, 0);
				CScopeGuard guard(_mutex);
				_fd_array.erase(ev.data.u64);
			}
			else if(ev.events & EPOLLIN)
			{
				LOG4CPLUS_TRACE(FLogger, "epoll readable, fd = " << ev.data.fd
							<< ", seqno = " << H32(ev.data.u64));
				event.type = EVENT_READ;

				_fd_array[ev.data.u64] = time(NULL);
			}
			else if(ev.events & EPOLLOUT)
			{
				LOG4CPLUS_TRACE(FLogger, "epoll writeable, fd = " << ev.data.fd
							<< ", seqno = " << H32(ev.data.u64));
				event.type = EVENT_WRITE;
			}

			_event_processor_ptr->addEvent(event);
		}

		time_t this_check = time(NULL);
		if(this_check - last_check >= CHECK_INTERVAL)
		{
			LOG4CPLUS_DEBUG(FLogger, "it's time to check fd active!");

			clean_inactive_fd();

			last_check = this_check;
		}
	}
}


/**
* brief: 清理非活跃的连接
*/
void EpollServer::clean_inactive_fd()
{
	typedef map<int64_t, time_t>::iterator Iterator;

	CScopeGuard guard(_mutex);
	
	time_t deadline = time(NULL) - INACTIVE_INTERVAL;

	vector<int64_t> array;
	Iterator iter = _fd_array.begin();
	for(; iter != _fd_array.end(); ++iter)
	{
		int fd = L32(iter->first);
		int seqno = H32(iter->first);

		if(iter->second > deadline)
			continue;

		_epoll.del(fd, 0, 0);
		//::close(fd);

		Event event = {'\0'};
		event.seqno = seqno;
		event.type = EVENT_CLOSE;
		event.timestamp = current_time_usec();

		_event_processor_ptr->addEvent(event);

		array.push_back(iter->first);

		LOG4CPLUS_DEBUG(FLogger, "session[" << seqno << "|" << fd << " become inactive,"
				<< "now : " << time(NULL) << ", last active: " << iter->second);
	}

	vector<int64_t>::iterator viter = array.begin();
	while(viter != array.end())
	{
		_fd_array.erase(*viter);

		++viter;
	}
}

/**
* brief: 判断连接是否已经长时间无请求
*
* @param value
*/
bool EpollServer::is_inactive(pair<int64_t,time_t> value)
{
	if(time(NULL) - value.second > INACTIVE_INTERVAL)
	{
		int fd = L32(value.first);
		int seqno = H32(value.first);

		LOG4CPLUS_DEBUG(FLogger, "session[" << seqno << "|" << fd << " become inactive,"
				<< "now : " << time(NULL) << ", last active: " << value.second);

		//_epoll.del(fd, 0, 0);
		//::close(fd);

		//Event event = {'\0'};
		//event.seqno = seqno;
		//event.type = EVENT_CLOSE;
		//event.timestamp = current_time_usec();

		//_event_processor_ptr->addEvent(event);

		return true;
	}

	return false;
}
