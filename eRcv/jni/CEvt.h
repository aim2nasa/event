#ifndef __CEVT_H__
#define __CEVT_H__

#include <list>

class CEvtProxy;
class IClassifyNoti;

class DLLEXPORT CEvt{
public:
	CEvt(bool aceInit=true);
	virtual ~CEvt();

	std::size_t addDevice(int n);
	std::size_t devices();
	std::list<int>& devList();
	void notify(IClassifyNoti *p);

	int open(const char *addr, unsigned short port);
	int close();

	int recordStart();
	int recordStop();
	int upPrepare(const char* file);
	int upload(const char* file);
	int play(const char* file);
	int play(const char* file, long long startLoc, long long endLoc);

	static long fileSize(const char* file);
	static long recordCount(const char* file);

protected:
	CEvtProxy *_proxy;
	bool _aceInit;
};

#endif