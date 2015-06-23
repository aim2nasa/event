#ifndef __CMTTRACKING_H__
#define __CMTTRACKING_H__

#include "CTracking.h"

class CMtTracking : public CTracking{
public:
	CMtTracking(){ reset(); }
	virtual void reset()
	{
		CTracking::reset();
		_count = _max = _swipe = 0;
		_index = _initialX = _initialY = _latestX = _latestY = -1;
	}

	int _index;
	int _count;
	int _max;
	int _initialX;
	int _initialY;
	int _latestX;
	int _latestY;
	bool _swipe;
};

#endif