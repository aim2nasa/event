#include "CEvtRec.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

#define DEVICE_FORMAT "/dev/input/event%d"

CEvtRec::CEvtRec()
:_pFds(NULL),_errDev(-1),_fdw(-1)
{
}

CEvtRec::~CEvtRec()
{
    delete [] _pFds;
}

std::size_t CEvtRec::addDevice(int n)
{
    _devList.push_back(n);
    return _devList.size();
}

std::size_t CEvtRec::devices()
{
    return _devList.size();
}

std::list<int>& CEvtRec::devList()
{
    return _devList;
}

int CEvtRec::errDev()
{
    return _errDev;
}

bool CEvtRec::devOpen()
{
    _errDev = -1;
    int i=0;
    char name[PATH_MAX];
    _POLLFD *pFds = new _POLLFD[_devList.size()];
    for(std::list<int>::iterator it=_devList.begin();it!=_devList.end();it++){
        sprintf(name,DEVICE_FORMAT,*it);
        pFds[i].events = POLLIN;
        pFds[i].fd = open(name,O_RDONLY|O_NDELAY);
        if(pFds[i].fd<0) {
            delete [] pFds;
            _errDev = *it;
            return false;
        }
        i++;
    }
    _pFds = pFds;
    return true;
}

bool CEvtRec::recOpen(const char *dumpFile)
{
    int fdw = open(dumpFile,O_WRONLY | O_CREAT | O_TRUNC,0644);
    if(fdw<0) return false;
    _fdw = fdw;
    return true; //don't want to let the outside to know fd
}

int CEvtRec::recClose()
{
    return close(_fdw);
}

int CEvtRec::rec()
{
    struct input_event event;
    ssize_t size;
    while(1)
    {
        if(poll(_pFds,_devList.size(),-1) <0) {
            return -1; //poll failed
        }

        std::list<int>::iterator it=_devList.begin();
        for(int i=0;i<_devList.size();i++) {
            if(_pFds[i].revents & POLLIN) {
                size = read(_pFds[i].fd,&event,sizeof(event));
                if(size!=sizeof(event)) {
                    return -2; //read error
                }
                if(dump(_fdw,*it,event)!=0) return -3;
            }
            it++;
        }
    }
}

int CEvtRec::dump(int fd,int device,struct input_event& event)
{
    if(write(fd,&device,sizeof(device))!=sizeof(device)) return -1;
    if(write(fd,&event,sizeof(event))!=sizeof(event)) return -2;
    return 0;
}
