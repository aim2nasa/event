#ifndef __CEVTREC_H__
#define __CEVTREC_H__

#include <list>
#include <poll.h>
#include <pthread.h>

#define REVT_OK        0
#define REVT_ERR_POLL -1
#define REVT_ERR_READ -2
#define REVT_ERR_DUMP -3

typedef pollfd _POLLFD;

class IEvtDump{
public:
    virtual ~IEvtDump(){}
    virtual int evtDmp(int dev,int type,int code,int value)=0; 
};

class CEvtRec{
protected:
    CEvtRec();
public:
    virtual ~CEvtRec();

    std::size_t addDevice(int n);
    std::size_t devices();
    std::list<int>& devList();
    void evtDump(IEvtDump *p);

    bool devOpen();
    int errDev();

    int start();
    int stop();
    int wait();

    static CEvtRec* instance();

protected:
    static void* readEvent(void *arg);

protected:
    std::list<int> _devList;
    _POLLFD *_pFds;
    int _errDev;
    IEvtDump *_pEvtDump;
    pthread_t _id;
    static CEvtRec *_sInstance;
};

#endif
