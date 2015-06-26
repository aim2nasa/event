#pragma once

#include "IClassifyNoti.h"

class CClassRes : public IClassifyNoti{
public:
	CClassRes();
	virtual ~CClassRes();
	void onNewDevice(int device, DEV_TYPE devType, int index);
	void onKeyEvent(int device, int startIndex, int endIndex);
	void onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex);
	void onError(ERR_CODE code);
};