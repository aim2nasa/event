#ifndef __CKEYTRACKING_H__
#define __CKEYTRACKING_H__

#include "CTracking.h"

class CKeyTracking : public CTracking{
public:
	CKeyTracking() { reset(); }
	virtual void reset()
	{
		CTracking::reset();
		_index = _device = _code = _value = -1;
	}

	int _index;
	int _device;
	int _code;
	int _value;
};

#endif