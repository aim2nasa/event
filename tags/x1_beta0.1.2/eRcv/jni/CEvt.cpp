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

int CEvt::open(const char *addr, unsigned short port)
{
	int rtn;
	if((rtn=_proxy->init(addr, port))!=0) return rtn;
	_proxy->start();
	return rtn;
}

int CEvt::close()
{
	return _proxy->stop();
}

void CEvt::playNoti(IPlayNoti *p)
{
	_proxy->playNoti(p);
}

int CEvt::recordStart(IClassifyNoti *p)
{
	return _proxy->recordStart(p);
}

int CEvt::recordStop()
{
	return _proxy->recordStop();
}

int CEvt::play(const char* file)
{
	return _proxy->play(file);
}

int CEvt::play(const char* file, long long startLoc, long long endLoc)
{
	return _proxy->play(file, startLoc, endLoc);
}

int CEvt::playFirst(const char* file)
{
	return _proxy->playFirst(file);
}

int CEvt::playNext()
{
	return _proxy->playNext();
}

long CEvt::fileSize(const char* file)
{
	return CEvtProxy::fileSize(file);
}

long CEvt::recordCount(const char* file)
{
	return CEvtProxy::recordCount(file);
}

int CEvt::stepCount(const char* file)
{
	if (_proxy->retrieveEventsInfo(file) != 0) return -1;
	return _proxy->eventsInfo()->list().size();
}