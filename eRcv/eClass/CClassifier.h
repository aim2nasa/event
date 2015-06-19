#ifndef __CCLASSIFIER_H__
#define __CCLASSIFIER_H__

#include <map>

//EVENT TYPES
#define EV_SYN		0x00
#define EV_KEY		0x01
#define EV_ABS		0x03

//Synchronization events
#define SYN_REPORT	0

//Absolute
#define ABS_MT_TRACKING_ID      0x39    /* Unique ID of initiated contact */

class CClassifier {
public:
	enum DEV_TYPE{ TOUCH, KEY };
	enum TOUCH_TYPE{ TAP, SWIPE, MULTITOUCH };

	CClassifier();
	virtual ~CClassifier();

	void reset();
	int addEvt(int device, long sec, long usec, int type, int code, int value);

protected:
	std::map<int, DEV_TYPE> _devMap;
};

#endif