#ifndef __CEVTPROXY_H__
#define __CEVTPROXY_H__

#include "ace/Task.h"
#include <list>
#include "ace/Auto_Event.h"

class ACE_SOCK_Stream;
class ACE_Auto_Event;
class CClassifier;
class IClassifyNoti;

class CEvtProxy : public ACE_Task < ACE_MT_SYNCH >
{
    CEvtProxy();
public:
    CEvtProxy(ACE_SOCK_Stream* p);
    ~CEvtProxy();

    std::size_t addDevice(int n);
    std::size_t devices();
    std::list<int>& devList();
    void notify(IClassifyNoti *p);

    int init();
    int start();
    int stop();

    int recordStart();
    int recordStop();
    int upPrepare(const char* file);
    int upload(const char* file);
    int play(const char* file);
    int play(const char* file,long long startLoc,long long endLoc);

    static long fileSize(const char* file);
protected:
    virtual int svc(void);
    int send(int msg);
    int send(std::list<int>& seq);
    int recv_int(int& msg);
    int onEventRecordData();
    static ACE_THR_FUNC_RETURN initResponse(void *p);

protected:
    ACE_SOCK_Stream* _pStream;
    std::list<int> _devList;
    ACE_Auto_Event _iEvt;
    ACE_Auto_Event _upEvt;
    char _buffer[BUFSIZ];
    FILE *_fp;
    long _fSize;
    CClassifier* _pClass;
    IClassifyNoti* _pNoti;
    long _index;
};

#endif