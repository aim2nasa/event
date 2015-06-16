#ifndef __CSENDER_H__
#define __CSENDER_H__

#include "CEvtRec.h"

class ACE_SOCK_Stream;

class CSender : public IEvtDump{
public:
    CSender();
    virtual ~CSender();

    int evtDmp(int dev,struct input_event& event);

    ACE_SOCK_Stream *_pPeer;
};

#endif
