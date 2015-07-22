#ifndef __CCLASSRESULT_H__
#define __CCLASSRESULT_H__

#include "CClassifier.h"
#include <stdio.h>

class CClassResult : public IClassifyNoti{
public:
	CClassResult();
	virtual ~CClassResult();

	bool open(const char *name);
	void close();

	void onNewDevice(int device, DEV_TYPE devType, int index);
	void onKeyEvent(int device, int code, int value, int startIndex, int endIndex);
	void onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex);
	void onError(ERR_CODE code);
protected:
	void writeIndex(DEV_TYPE devType,int device,int startIndex, int endIndex);

protected:
	FILE* _fp;
};

#endif
