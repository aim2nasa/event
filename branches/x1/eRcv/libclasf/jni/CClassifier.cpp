#include "CClassifier.h"
#include "macro.h"
#include "CMtTracking.h"
#include "CKeyTracking.h"
#include "ace/Log_Msg.h"
#include <stdlib.h>

CClassifier::CClassifier()
	:_pDevMap(new DEVMAP()),_pMt(new CMtTracking()), _pKt(new CKeyTracking()), _pNoti(NULL)
{
	
}

CClassifier::~CClassifier()
{
	delete _pKt;
	delete _pMt;
	delete _pDevMap;
}

CClassifier::DEVMAP& CClassifier::devMap()
{
	return *_pDevMap;
}

void CClassifier::notify(IClassifyNoti *p)
{
	_pNoti = p;
}

void CClassifier::reset()
{
	_pDevMap->clear();
	_pMt->reset();
	_pKt->reset();
}

bool CClassifier::isTouchDevice(int device)
{
	DEVMAP::iterator it = _pDevMap->find(device);
	if (it == _pDevMap->end()) return false;
	if (it->second == IClassifyNoti::TOUCH) return true;
	return false;
}

int CClassifier::addEvt(long index, int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] index:%d,dev:%02d,time:%d.%06d,type:%04x,code:%04x,val:%08x\n",index,device,sec,usec,type,code,value));

	std::pair<DEVMAP::iterator, bool> ret;
	if (EV_KEY == type) {
		if (_pDevMap->insert(std::make_pair(device, IClassifyNoti::KEY)).second)
			onNewKeyDevice(index,device,sec,usec,type,code,value);
		else
			onExistingKeyDevice(index, device, sec, usec, type, code, value);

		onKey(index, device, sec, usec, type, code, value);
	}else if (EV_ABS == type){
		if (_pDevMap->insert(std::make_pair(device, IClassifyNoti::TOUCH)).second)
			onNewTouchDevice(index, device, sec, usec, type, code, value);
		else
			onExistingTouchDevice(index, device, sec, usec, type, code, value);

		if (code == ABS_MT_TRACKING_ID) onAbsMtTrackingId(index,device,sec,usec,type,code,value);
		if (code == ABS_MT_POSITION_X) onAbsMtpositionX(index, device, sec, usec, type, code, value);
		if (code == ABS_MT_POSITION_Y) onAbsMtpositionY(index, device, sec, usec, type, code, value);
	}else if (EV_SYN == type){
		if (code == SYN_REPORT) onSynReport(index, device, sec, usec, type, code, value);
	}
	return 0;
}

void CClassifier::onNewKeyDevice(long index, int device, long sec, long usec, int type, int code, int value)
{
	if(_pNoti) _pNoti->onNewDevice(device, IClassifyNoti::KEY, index);
	ACE_DEBUG((LM_DEBUG, ">> index:%d,dev:%02d type:%04x KEY TYPE registered(map size:%d)\n", index, device, type, _pDevMap->size()));
}

void CClassifier::onExistingKeyDevice(long index, int device, long sec, long usec, int type, int code, int value)
{

}

void CClassifier::onKey(long index, int device, long sec, long usec, int type, int code, int value)
{
	if (isTouchDevice(device)) {
		if (((code >= 0x0) && (code <= 0xff)) || ((code >= 0x160) && (code <= 0x265))) {
			//KEY_RESERVED~RESERVED , KEY_OK 0x160 ~ KEY_KBDINPUTASSIST_CANCEL 0x265
			_pKt->set(true, index, device, code, value);
		}
		ACE_DEBUG((LM_DEBUG, "[%T] Key event(code:%04x,val:%08x) from Touch device(%d), neglect\n", code,value,device));
		return;	//터치장치에서 키이벤트가 발생을 하면 무시하도록 한다 (삼성폰등에서 발생됨)
	}else{
		if (_pMt->_tracking) {
			//키이벤트가 들어올때 터치장치에서의 추적은 반드시 꺼져 있어야 한다. 현버전은 스와이프나 탭이 끝나지 않은 경우에 다른 장치 이벤트가 들어오는것을 가정하지 않는다
			//이러한 제약을 없애려면 각장치에서 오는 이벤트 녹화및 재생을 각 장치별로 스레드를 만들어 처리해야 하는데, 이는 UI상에서도 이벤트 단위 구분이 바뀌어야 하므로 에러로만 처리한다.
			if (_pNoti) _pNoti->onError(IClassifyNoti::DEV_MIXED);
			return;
		}
	}
	_pKt->set(true,index,device,code,value);
}

void CClassifier::onNewTouchDevice(long index, int device, long sec, long usec, int type, int code, int value)
{
	if (_pNoti) _pNoti->onNewDevice(device, IClassifyNoti::TOUCH, index);
	ACE_DEBUG((LM_DEBUG, ">> index:%d,dev:%02d type:%04x TOUCH TYPE registered(map size:%d)\n", index, device, type, _pDevMap->size()));
}

void CClassifier::onExistingTouchDevice(long index, int device, long sec, long usec, int type, int code, int value)
{

}

void CClassifier::onAbsMtTrackingId(long index, int device, long sec, long usec, int type, int code, int value)
{
	if (_pNoti) _pNoti->onAbsTrackingID(value, _pMt->_count);
	if (value != 0xffffffff) {
		if (_pMt->_count == 0) {
			_pMt->_value = value;
			_pMt->_index = index;
			_pMt->_tracking = true;
		}

	
		if (_pMt->_count > 0 && (value<_pMt->_value)) {
			//LG G Flex와 LG Vu3에서는 ABS_MT_TRACKING_ID사이에 ABS_MT_TRACKING_ID가 -1이 아닌 값이 나타나서 이 경우에는 카운트를 하지 않는다
			ACE_DEBUG((LM_DEBUG, "[%T] MT Tracking(%d), Skip count value(prev:%d,cur:%d)\n", _pMt->_count, _pMt->_value,value));
		}else{
			_pMt->_max = ++_pMt->_count;
			ACE_DEBUG((LM_DEBUG, "[%T] MT Tracking(%d) started, val(%08x)\n", _pMt->_count, value));
		}
	}else{
		_pMt->_count--;
		ACE_DEBUG((LM_DEBUG, "[%T] MT Tracking(%d), val(%08x)\n", _pMt->_count, value));
	}
}

void CClassifier::onSynReport(long index, int device, long sec, long usec, int type, int code, int value)
{
	if (_pMt->_tracking) {
		if (_pMt->_count == 0) {
			if (_pNoti) _pNoti->onTouchEvent(device,(_pMt->_swipe) ? IClassifyNoti::SWIPE : IClassifyNoti::TAP, _pMt->_index, index);
			ACE_DEBUG((LM_DEBUG, "[%T] MT Tracking(%d) done, MaxTouch(%d) %s,%s\n", _pMt->_count, _pMt->_max, (_pMt->_max>1) ? "Multi-touch" : "Single-touch",(_pMt->_swipe)?"SWIPE":"TAP"));
			_pMt->reset();
		}
	}

	if (_pKt->_tracking) {
		if (_pNoti) {
			if((_pKt->_index+1)!=index) _pNoti->onError(IClassifyNoti::KEY_SEQ);	//키이벤트의 인덱스는 순차적으로 2개만 들어오는 것을 가정한다.(즉, 키를 누르고 SYN까지를 하나의 단위로 처리,관찰한바 항상 2개를 단위로 처리됨)
			_pNoti->onKeyEvent(device,_pKt->_code,_pKt->_value,_pKt->_index, index);
		}
		ACE_DEBUG((LM_DEBUG, "[%T] Key(dev:%02d,code:%04x,val:%08x) Tracking done\n",_pKt->_device,_pKt->_code,_pKt->_value));
		_pKt->reset();
	}
}

void CClassifier::onAbsMtpositionX(long index, int device, long sec, long usec, int type, int code, int value)
{
	if (_pMt->_initialX == -1) _pMt->_initialX = value;
	_pMt->_latestX = value;
	if (abs(_pMt->_initialX - _pMt->_latestX) > MAX_TAP_DISTANCE) _pMt->_swipe = true;
}

void CClassifier::onAbsMtpositionY(long index, int device, long sec, long usec, int type, int code, int value)
{
	if (_pMt->_initialY == -1) _pMt->_initialY = value;
	_pMt->_latestY = value;
	if (abs(_pMt->_initialY - _pMt->_latestY) > MAX_TAP_DISTANCE) _pMt->_swipe = true;
}
