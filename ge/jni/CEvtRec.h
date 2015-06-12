#ifndef __CEVTREC_H__
#define __CEVTREC_H__

#include <list>
#include <poll.h>

typedef pollfd _POLLFD;

class IEvtDump{
public:
    virtual ~IEvtDump(){}
    virtual int evtDmp(int dev,int type,int code,int value)=0; 
};

class CEvtRec{
public:
    CEvtRec();
    ~CEvtRec();

    std::size_t addDevice(int n);
    std::size_t devices();
    std::list<int>& devList();

    bool devOpen();
    int errDev();

    bool recOpen(const char *dumpFile);
    int recClose();
    int rec();

private:
    static int dump(int fd,int device,struct input_event& event);

private:
    std::list<int> _devList;
    _POLLFD *_pFds;
    int _errDev;
    int _fdw;
};

#endif
