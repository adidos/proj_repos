
/* ======================================================
* 
* file:		fd_reactor.cpp
* brief:	
* author:	80070525(chenjian)
* version:	1.0.0
* date:		2014-06-15 23:46:10
* 
* ======================================================*/

#include "fd_reactor.h"

FDReactor::FDReactor()
	:_terminate(false)
{
	_ep.create(1024);
}

FDReactor::~FDReactor()
{

}

void FDReactor::notify(int fd)
{
	_ep.mod(fd, fd , EPOLLIN | EPOLLOUT);
}

void FDReactor::doIt()
{

}

void FDReactor::handle(int fd, events)
{


}
