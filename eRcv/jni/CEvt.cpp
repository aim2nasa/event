#include "CEvt.h"
#include "CEvtProxy.h"
#include "ace/Init_ACE.h"

CEvt::CEvt(bool aceInit)
	:_proxy(new CEvtProxy()), _aceInit(aceInit)
{
	if(_aceInit) ACE::init();
}

CEvt::~CEvt()
{
	delete _proxy;
	if (_aceInit) ACE::fini();
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

int CEvt::open(const char *addr, unsigned short port)
{
	int rtn;
	if((rtn=_proxy->init(addr, port))<=0) return rtn;
	_proxy->start();
	return rtn;
}

int CEvt::close()
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

long CEvt::recordCount(const char* file)
{
	return CEvtProxy::recordCount(file);
}