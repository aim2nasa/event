#ifndef __CCLASSIFIER_H__
#define __CCLASSIFIER_H__

#include "macro.h"
#include <map>
#include "IClassifyNoti.h"

class CMtTracking;
class CKeyTracking;

class DLLEXPORT CClassifier {
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
	DEVMAP *_pDevMap;
	CMtTracking *_pMt;
	CKeyTracking *_pKt;
	IClassifyNoti *_pNoti;
};

#endif