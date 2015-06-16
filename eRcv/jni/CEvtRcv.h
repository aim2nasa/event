#ifndef __CEVTRCV_H__
#define __CEVTRCV_H__

#include "ace/Task.h"
#include <list>
#include "ace/Auto_Event.h"

class ACE_SOCK_Stream;
class ACE_Auto_Event;

class CEvtRcv : public ACE_Task < ACE_MT_SYNCH >
{
    CEvtRcv();
public:
    CEvtRcv(ACE_SOCK_Stream* p);
    ~CEvtRcv();

    std::size_t addDevice(int n);
    std::size_t devices();
    std::list<int>& devList();

    int init();
    int start();
    int stop();

    int send(int msg);
    int send(std::list<int>& seq);
    int recv_int(int& msg);
    int recordStart();
    int recordStop();

    int onEventRecordData();

    virtual int svc(void);

    static ACE_THR_FUNC_RETURN initResponse(void *p);

protected:
    ACE_SOCK_Stream* _pStream;
    std::list<int> _devList;
    ACE_Auto_Event _iEvt;
    char _buffer[BUFSIZ];
};

#endif
