#ifndef __CTRACKING_H__
#define __CTRACKING_H__

class CTracking{
public:
	CTracking() { reset(); }
	virtual void reset() { _tracking = 0; }

	bool _tracking;
};

#endif