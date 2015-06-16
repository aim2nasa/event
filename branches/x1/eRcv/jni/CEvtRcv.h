#ifndef __CEVTRCV_H__
#define __CEVTRCV_H__

#include "ace/Task.h"
#include <list>

class ACE_SOCK_Stream;

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
    int recordStart();
    int recordStop();

    virtual int svc(void);

protected:
    ACE_SOCK_Stream* _pStream;
    std::list<int> _devList;
};

#endif
