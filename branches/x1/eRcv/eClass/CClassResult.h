#ifndef __CCLASSRESULT_H__
#define __CCLASSRESULT_H__

#include "CClassifier.h"

class CClassResult : public IClassifyNoti{
public:
	CClassResult();
	virtual ~CClassResult();

	bool open(const char *name);
	void close();

	void onNewDevice(int device, DEV_TYPE devType, int index);
	void onKeyEvent(int startIndex, int endIndex);
	void onTouchEvent(TOUCH_TYPE type, int startIndex, int endIndex);
	void onError(ERR_CODE code);
protected:
	void writeIndex(DEV_TYPE devType,int startIndex, int endIndex);

protected:
	FILE* _fp;
};

#endif