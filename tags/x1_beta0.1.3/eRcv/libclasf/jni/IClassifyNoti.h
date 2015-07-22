#ifndef __ICLASSIFYNOTI_H__
#define __ICLASSIFYNOTI_H__

class DLLEXPORT IClassifyNoti{
public:
	enum DEV_TYPE{ TOUCH, KEY };
	enum TOUCH_TYPE{ INVALID_TOUCH=-1,TAP, SWIPE, MULTITOUCH };
	enum ERR_CODE{ DEV_MIXED, KEY_SEQ };

	virtual ~IClassifyNoti(){}

	virtual void onNewDevice(int device, DEV_TYPE type, int index) = 0;
	virtual void onKeyEvent(int device, int code, int value, int startIndex, int endIndex) = 0;
	virtual void onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex) = 0;
	virtual void onError(ERR_CODE code) = 0;
};

#endif