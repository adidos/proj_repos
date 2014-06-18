#include "transceiver_handle.h"


TransceiverHandle::TransceiverHandle()
{

}

TransceiverHandle::~TransceiverHandle()
{

}

void TransceiverHandle::handle(int fd, int evs)
{
	AdapterProxy* ap = NULL;
	{
		CScopeGuard guard(_mutex);
		Iteartor iter = _proxys.find(fd);

		if(iter != _proxys.end())
		{
			ap = iter->second;
		}
	}

	if(NULL == ap)
		return;
	
	if(evs &  TransceiverHandle::W)
	{
		handleOutput(ap);
	}

	if(evs & TransceiverHandle::R)
	{
		handleInput(ap);
	}
}

int TransceiverHandle::handleOutput(AdapterProxy* ap)
{
	if(NULL == ap)
		reutrn -1;

	ap->sendRequest();
}

int TransceiverHandle::handleInput(AdapterProxy* ap)
{
	if(NULL == ap)
		reutrn -1;

	ap->sendRequest();
}