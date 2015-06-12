#ifndef __CDUMP_H__
#define __CDUMP_H__

#include "CEvtRec.h"

class CDump : public IEvtDump{
public:
    CDump();
    virtual ~CDump();

    bool dumpOpen(const char *file);
    int dumpClose();
    int evtDmp(int dev,int type,int code,int value);

protected:
    int _fd;
};

#endif
