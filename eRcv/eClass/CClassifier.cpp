#include "CClassifier.h"
#include "ace/Log_Msg.h"

CClassifier::CClassifier()
:_prevDev(-1)
{

}

CClassifier::~CClassifier()
{

}

void CClassifier::reset()
{
	_devMap.clear();
	_prevDev = -1;
}

int CClassifier::addEvt(long evtOrder,int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] order:%d,dev:%02d,time:%d.%06d,type:%04x,code:%04x,val:%08x\n",evtOrder,device,sec,usec,type,code,value));

	std::pair<std::map<int,DEV_TYPE>::iterator, bool> ret;
	if (EV_KEY == (DEV_TYPE)type) {
		if (_devMap.insert(std::make_pair(device, KEY)).second)
			onNewKeyDevice(evtOrder,device,sec,usec,type,code,value);
		else
			onExistingKeyDevice(evtOrder, device, sec, usec, type, code, value);
			
	}else if (EV_ABS == (DEV_TYPE)type){
		if (_devMap.insert(std::make_pair(device, TOUCH)).second)
			onNewTouchDevice(evtOrder, device, sec, usec, type, code, value);
		else
			onExistingTouchDevice(evtOrder, device, sec, usec, type, code, value);
	}

	_prevDev = device;
	return 0;
}

void CClassifier::onNewKeyDevice(long evtOrder, int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] order:%d,dev:%02d type:%04x KEY TYPE registered(map size:%d)\n", evtOrder, device, type, _devMap.size()));
}

void CClassifier::onExistingKeyDevice(long evtOrder, int device, long sec, long usec, int type, int code, int value)
{

}

void CClassifier::onNewTouchDevice(long evtOrder, int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] order:%d,dev:%02d type:%04x TOUCH TYPE registered(map size:%d)\n", evtOrder, device, type, _devMap.size()));
}

void CClassifier::onExistingTouchDevice(long evtOrder, int device, long sec, long usec, int type, int code, int value)
{

}