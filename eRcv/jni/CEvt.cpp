#include "CEvt.h"
#include "CEvtProxy.h"

CEvt::CEvt()
:_proxy(new CEvtProxy())
{

}

CEvt::~CEvt()
{
	delete _proxy;
}

std::size_t CEvt::addDevice(int n)
{
	return _proxy->addDevice(n);
}

std::size_t CEvt::devices()
{
	return _proxy->devices();
}

std::list<int>& CEvt::devList()
{
	return _proxy->devList();
}

void CEvt::notify(IClassifyNoti *p)
{
	_proxy->notify(p);
}

int CEvt::init(const char *addr, unsigned short port)
{
	return _proxy->init(addr, port);
}

int CEvt::start()
{
	return _proxy->start();
}

int CEvt::stop()
{
	return _proxy->stop();
}

int CEvt::recordStart()
{
	return _proxy->recordStart();
}

int CEvt::recordStop()
{
	return _proxy->recordStop();
}
	
int CEvt::upPrepare(const char* file)
{
	return _proxy->upPrepare(file);
}

int CEvt::upload(const char* file)
{
	return _proxy->upload(file);
}

int CEvt::play(const char* file)
{
	return _proxy->play(file);
}

int CEvt::play(const char* file, long long startLoc, long long endLoc)
{
	return _proxy->play(file, startLoc, endLoc);
}

long CEvt::fileSize(const char* file)
{
	return CEvtProxy::fileSize(file);
}