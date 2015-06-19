#ifndef __CCLASSRESULT_H__
#define __CCLASSRESULT_H__

#include "CClassifier.h"

class CClassResult : public IClassifyNoti{
public:
	CClassResult();
	virtual ~CClassResult();

	void onKeyEvent(int startIndex, int endIndex);
	void onTouchEvent(TOUCH_TYPE type, int startIndex, int endIndex);
};

#endif