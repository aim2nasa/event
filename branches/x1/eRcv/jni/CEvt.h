#ifndef __CEVT_H__
#define __CEVT_H__

#include <list>

class CEvtProxy;
class IClassifyNoti;

class DLLEXPORT CEvt{
public:
	CEvt(bool aceInit=true);
	virtual ~CEvt();

	int open(const char *addr, unsigned short port);
	int close();

	int recordStart(IClassifyNoti *p);
	int recordStop();
	int play(const char* file);
	int play(const char* file, long long startLoc, long long endLoc);

	static long fileSize(const char* file);
	static long recordCount(const char* file);

protected:
	CEvtProxy *_proxy;
	bool _aceInit;
};

#endif