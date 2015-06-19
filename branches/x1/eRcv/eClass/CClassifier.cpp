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

int CClassifier::addEvt(long index, int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] index:%d,dev:%02d,time:%d.%06d,type:%04x,code:%04x,val:%08x\n",index,device,sec,usec,type,code,value));

	std::pair<std::map<int,DEV_TYPE>::iterator, bool> ret;
	if (EV_KEY == (DEV_TYPE)type) {
		if (_devMap.insert(std::make_pair(device, KEY)).second)
			onNewKeyDevice(index,device,sec,usec,type,code,value);
		else
			onExistingKeyDevice(index, device, sec, usec, type, code, value);
			
	}else if (EV_ABS == (DEV_TYPE)type){
		if (_devMap.insert(std::make_pair(device, TOUCH)).second)
			onNewTouchDevice(index, device, sec, usec, type, code, value);
		else
			onExistingTouchDevice(index, device, sec, usec, type, code, value);
	}

	_prevDev = device;
	return 0;
}

void CClassifier::onNewKeyDevice(long index, int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] index:%d,dev:%02d type:%04x KEY TYPE registered(map size:%d)\n", index, device, type, _devMap.size()));
}

void CClassifier::onExistingKeyDevice(long index, int device, long sec, long usec, int type, int code, int value)
{

}

void CClassifier::onNewTouchDevice(long index, int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] index:%d,dev:%02d type:%04x TOUCH TYPE registered(map size:%d)\n", index, device, type, _devMap.size()));
}

void CClassifier::onExistingTouchDevice(long index, int device, long sec, long usec, int type, int code, int value)
{

}