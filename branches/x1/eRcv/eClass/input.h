#ifndef __INPUT_H__
#define __INPUT_H__

#include "typedef.h"
#include "ace/Time_Value.h"

struct input_event {
	struct timeval time;
	_u16 type;
	_u16 code;
	_s32 value;
};

#endif