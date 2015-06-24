#ifndef __CCLASSRESULT_H__
#define __CCLASSRESULT_H__

#include "CClassifier.h"

class CClassResult : public IClassifyNoti{
public:
	CClassResult();
	virtual ~CClassResult();

	void onNewDevice(int device, DEV_TYPE devType, int index);
	void onKeyEvent(int device, int startIndex, int endIndex);
	void onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex);
	void onError(ERR_CODE code);
};

#endif
