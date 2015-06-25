#ifndef __CEVTPROXY_H__
#define __CEVTPROXY_H__

#include "ace/Task.h"
#include <list>
#include "ace/Auto_Event.h"

class ACE_SOCK_Stream;
class ACE_Auto_Event;
class CClassifier;
class IClassifyNoti;
class CResult;

class DLLEXPORT CEvtProxy : public ACE_Task < ACE_MT_SYNCH >
{
public:
    CEvtProxy();
    virtual ~CEvtProxy();

    int init(const char *addr,unsigned short port);
    int start();
    int stop();

	CResult *eventsInfo();
	int recordStart(IClassifyNoti *p);
    int recordStop();
    int upPrepare(const char* file);
    int upload(const char* file);
    int play(const char* file);
    int play(const char* file,long long startLoc,long long endLoc);

    static long fileSize(const char* file);
	static long recordCount(const char* file);
protected:
    virtual int svc(void);
    int send(int msg);
    int send(std::list<int>& seq);
    int recv_int(int& msg);
    int onEventRecordData();
    static ACE_THR_FUNC_RETURN initResponse(void *p);

protected:
    void* _pStream;
    ACE_Auto_Event _iEvt;
    ACE_Auto_Event _upEvt;
    FILE *_fp;
    long _fSize;
    CClassifier* _pClass;
    long _index;
	CResult *_pEventsInfo;
};

#endif
