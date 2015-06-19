#ifndef __CCLASSIFIER_H__
#define __CCLASSIFIER_H__

#include <map>
#include "ace/Time_Value.h"

//EVENT TYPES
#define EV_SYN		0x00
#define EV_KEY		0x01
#define EV_ABS		0x03

//Synchronization events
#define SYN_REPORT	0

//Absolute
#define ABS_MT_TRACKING_ID      0x39    /* Unique ID of initiated contact */

typedef unsigned short _u16;
typedef unsigned int _u32;
typedef short _s16;
typedef int _s32;

struct input_event {
	struct timeval time;
	_u16 type;
	_u16 code;
	_s32 value;
};

class CRecord {
public:
	CRecord() :_device(-1){ memset(&_event, -1, sizeof(_event)); }

	_u32 _device;
	struct input_event _event;
};

class CClassifier {
public:
	enum DEV_TYPE{ TOUCH, KEY };
	enum TOUCH_TYPE{ TAP, SWIPE, MULTITOUCH };

	CClassifier();
	virtual ~CClassifier();

	void reset();
	int addEvt(long index,int device, long sec, long usec, int type, int code, int value);

protected:
	void resetMtTracking();
	void onNewKeyDevice(long index, int device, long sec, long usec, int type, int code, int value);
	void onExistingKeyDevice(long index, int device, long sec, long usec, int type, int code, int value);
	void onNewTouchDevice(long index, int device, long sec, long usec, int type, int code, int value);
	void onExistingTouchDevice(long index, int device, long sec, long usec, int type, int code, int value);
	void onAbsMtTrackingId(long index, int device, long sec, long usec, int type, int code, int value);
	void onSynReport(long index, int device, long sec, long usec, int type, int code, int value);

protected:
	std::map<int, DEV_TYPE> _devMap;
	int _mtTrack;
	int _mtMax;
	bool _onMtTracking;
};

#endif