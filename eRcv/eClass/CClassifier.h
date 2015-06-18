#ifndef __CCLASSIFIER_H__
#define __CCLASSIFIER_H__

class CClassifier {
public:
	CClassifier();
	virtual ~CClassifier();

	int addEvt(int device, long sec, long usec, int type, int code, int value);
};

#endif