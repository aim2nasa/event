#ifndef __CCLASSIFIER_H__
#define __CCLASSIFIER_H__

#include <map>

class CMtTracking;
class CKeyTracking;

class CClassifier {
public:
	enum DEV_TYPE{ TOUCH, KEY };
	enum TOUCH_TYPE{ TAP, SWIPE, MULTITOUCH };

	typedef std::map<int, DEV_TYPE> DEVMAP;

	CClassifier();
	virtual ~CClassifier();

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
};

#endif