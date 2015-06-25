#ifndef __CFILEINFO_H__
#define __CFILEINFO_H__

#include <list>
#include "IClassifyNoti.h"

class DLLEXPORT CUserEvent{
	CUserEvent();
public:
	CUserEvent(IClassifyNoti::DEV_TYPE dt,int dev,int startIdx,int endIdx)
		:_devType(dt), _device(dev), _startIndex(startIdx), _endIndex(endIdx){}

	IClassifyNoti::DEV_TYPE _devType;
	int _device;
	int _startIndex;
	int _endIndex;
};

typedef std::list<CUserEvent> UEVT_LIST;

class DLLEXPORT CResult : public IClassifyNoti{
public:
	CResult();
	virtual ~CResult();

	void reset();
	UEVT_LIST& list();

protected:
	void onNewDevice(int device, DEV_TYPE type, int index);
	void onKeyEvent(int device, int startIndex, int endIndex);
	void onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex);
	void onError(ERR_CODE code);

	UEVT_LIST *_pList;
};

class DLLEXPORT CFileInfo {
public:
	static int analyze(const char *file,CResult *result);
};

#endif