#ifndef __CRECORD_H__
#define __CRECORD_H__

#include "typedef.h"
#include "input.h"

class CRecord {
public:
	CRecord() :_device(-1){ memset(&_event, -1, sizeof(_event)); }

	_u32 _device;
	struct input_event _event;
};

#endif