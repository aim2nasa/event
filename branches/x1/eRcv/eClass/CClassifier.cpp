#include "CClassifier.h"
#include "ace/Log_Msg.h"

CClassifier::CClassifier()
{

}

CClassifier::~CClassifier()
{

}

void CClassifier::reset()
{
	_devMap.clear();
	_mt.reset();
}

int CClassifier::addEvt(long index, int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] index:%d,dev:%02d,time:%d.%06d,type:%04x,code:%04x,val:%08x\n",index,device,sec,usec,type,code,value));

	std::pair<std::map<int,DEV_TYPE>::iterator, bool> ret;
	if (EV_KEY == type) {
		if (_devMap.insert(std::make_pair(device, KEY)).second)
			onNewKeyDevice(index,device,sec,usec,type,code,value);
		else
			onExistingKeyDevice(index, device, sec, usec, type, code, value);
			
	}else if (EV_ABS == type){
		if (_devMap.insert(std::make_pair(device, TOUCH)).second)
			onNewTouchDevice(index, device, sec, usec, type, code, value);
		else
			onExistingTouchDevice(index, device, sec, usec, type, code, value);

		if (code == ABS_MT_TRACKING_ID) onAbsMtTrackingId(index,device,sec,usec,type,code,value);
	}else if (EV_SYN == type){
		if (code == SYN_REPORT) onSynReport(index, device, sec, usec, type, code, value);
	}
	return 0;
}

void CClassifier::onNewKeyDevice(long index, int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, ">> index:%d,dev:%02d type:%04x KEY TYPE registered(map size:%d)\n", index, device, type, _devMap.size()));
}

void CClassifier::onExistingKeyDevice(long index, int device, long sec, long usec, int type, int code, int value)
{

}

void CClassifier::onNewTouchDevice(long index, int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, ">> index:%d,dev:%02d type:%04x TOUCH TYPE registered(map size:%d)\n", index, device, type, _devMap.size()));
}

void CClassifier::onExistingTouchDevice(long index, int device, long sec, long usec, int type, int code, int value)
{

}

void CClassifier::onAbsMtTrackingId(long index, int device, long sec, long usec, int type, int code, int value)
{
	if (value != 0xffffffff) {
		if (_mt._count == 0) _mt._tracking = true;
		_mt._max = ++_mt._count;
		ACE_DEBUG((LM_DEBUG, "[%T] MT Tracking(%d) started, val(%08x)\n", _mt._count, value));
	}else{
		_mt._count--;
		ACE_DEBUG((LM_DEBUG, "[%T] MT Tracking(%d), val(%08x)\n", _mt._count, value));
	}
}

void CClassifier::onSynReport(long index, int device, long sec, long usec, int type, int code, int value)
{
	if (_mt._tracking) {
		if (_mt._count == 0) {
			ACE_DEBUG((LM_DEBUG, "[%T] MT Tracking(%d) done, MaxTouch(%d) %s\n", _mt._count, _mt._max, (_mt._max>1) ? "Multi-touch" : "Single-touch"));
			_mt.reset();
		}
	}
}