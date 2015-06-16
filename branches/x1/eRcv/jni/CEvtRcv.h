#ifndef __CEVTRCV_H__
#define __CEVTRCV_H__

#include "ace/Task.h"
#include <vector>

class ACE_SOCK_Stream;

class CEvtRcv : public ACE_Task < ACE_MT_SYNCH >
{
    CEvtRcv();
public:
    CEvtRcv(ACE_SOCK_Stream* p);
    ~CEvtRcv();

    int start();
    int stop();

    virtual int svc(void);

private:
    ACE_SOCK_Stream* _pStream;
};

#endif
