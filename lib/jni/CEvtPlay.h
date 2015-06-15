#ifndef __CEVETPLAY_H__
#define __CEVTPLAY_H__

class IEvtPlay{
public:
    virtual ~IEvtPlay(){}
    virtual int play(int order,int dev,long sec,long uSec,int type,int code,int value)=0;
};

class CEvtPlay{
public:
    CEvtPlay();
    virtual ~CEvtPlay();

    int init(const char *file,IEvtPlay *p=0);
    long long fileSize();
    long long events();
    int play(long long maxRead);

protected:
    int _fd;
    long long _fileSize;
    long long _count;
    IEvtPlay *_pEvtPlay;
};

#endif
