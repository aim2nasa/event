#include "CEvtRec.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

#define DEVICE_FORMAT "/dev/input/event%d"

CEvtRec *CEvtRec::_sInstance=NULL;

CEvtRec* CEvtRec::instance()
{
    if(!_sInstance) _sInstance = new CEvtRec();
    return _sInstance;
}

CEvtRec::CEvtRec()
:_pFds(NULL),_errDev(-1),_pEvtDump(NULL),_id(-1)
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

void CEvtRec::evtDump(IEvtDump *p)
{
    _pEvtDump = p;
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

void* CEvtRec::readEvent(void *arg)
{
    CEvtRec *p = reinterpret_cast<CEvtRec*>(arg);

    struct input_event event;
    ssize_t size;
    while(1)
    {
        if(poll(p->_pFds,p->_devList.size(),-1) <0) {
            return (void*)REVT_ERR_POLL; //poll failed
        }

        std::list<int>::iterator it=p->_devList.begin();
        for(int i=0;i<p->_devList.size();i++) {
            if(p->_pFds[i].revents & POLLIN) {
                size = read(p->_pFds[i].fd,&event,sizeof(event));
                if(size!=sizeof(event)) {
                    return (void*)REVT_ERR_READ; //read error
                }
                if(p->_pEvtDump)
                    if(p->_pEvtDump->evtDmp(*it,event.type,event.code,event.value)!=0)
                        return (void*)REVT_ERR_DUMP;
            }
            it++;
        }
    }
}

int CEvtRec::start()
{
    return pthread_create(&_id,0,readEvent,this);
}

int CEvtRec::stop()
{
    return pthread_kill(_id,SIGUSR1);
}

int CEvtRec::wait()
{
    int status=0;
    return pthread_join(_id,(void**)&status);
}
