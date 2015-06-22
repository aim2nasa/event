#ifndef __CCLASSIFIER_H__
#define __CCLASSIFIER_H__

#include <map>

class CMtTracking;
class CKeyTracking;

class IClassifyNoti{
public:
	enum DEV_TYPE{ TOUCH, KEY };
	enum TOUCH_TYPE{ TAP, SWIPE, MULTITOUCH };
	enum ERR_CODE{ DEV_MIXED,KEY_SEQ };

	virtual ~IClassifyNoti(){}

	virtual void onKeyEvent(int startIndex,int endIndex)=0;
	virtual void onTouchEvent(TOUCH_TYPE type, int startIndex, int endIndex) = 0;
	virtual void onError(ERR_CODE code) = 0;
};

class CClassifier {
public:
	typedef std::map<int, IClassifyNoti::DEV_TYPE> DEVMAP;

	CClassifier();
	virtual ~CClassifier();

	DEVMAP& devMap();
	void notify(IClassifyNoti *p);
	void reset();
	int addEvt(long index,int device, long sec, long usec, int type, int code, int value);

protected:
	bool isTouchDevice(int device);
	void onNewKeyDevice(long index, int device, long sec, long usec, int type, int code, int value);
	void onExistingKeyDevice(long index, int device, long sec, long usec, int type, int code, int value);
	void onKey(long index, int device, long sec, long usec, int type, int code, int value);
	void onNewTouchDevice(long index, int device, long sec, long usec, int type, int code, int value);
	void onExistingTouchDevice(long index, int device, long sec, long usec, int type, int code, int value);
	void onAbsMtTrackingId(long index, int device, long sec, long usec, int type, int code, int value);
	void onSynReport(long index, int device, long sec, long usec, int type, int code, int value);
	void onAbsMtpositionX(long index, int device, long sec, long usec, int type, int code, int value);
	void onAbsMtpositionY(long index, int device, long sec, long usec, int type, int code, int value);

protected:
	DEVMAP _devMap;
	CMtTracking *_pMt;
	CKeyTracking *_pKt;
	IClassifyNoti *_pNoti;
};

#endif