#include "CEvtRec.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <dirent.h>

#define DEVICE_PATH "/dev/input"
#define DEVICE_FORMAT "/dev/input/event%d"

CEvtRec *CEvtRec::_sInstance=NULL;

CEvtRec* CEvtRec::instance()
{
    if(!_sInstance) _sInstance = new CEvtRec();
    return _sInstance;
}

void CEvtRec::delInstance()
{
    delete _sInstance;
    _sInstance = NULL;
}

CEvtRec::CEvtRec()
:_pFds(NULL),_errDev(-1),_pEvtDump(NULL),_id(-1)
{
    memset(_fdPipe,-1,sizeof(_fdPipe));
    _devList.push_back(-1); //for internal pipe
}

CEvtRec::~CEvtRec()
{
    delete [] _pFds;
}

std::size_t CEvtRec::addDevice(int n)
{
    _devList.push_back(n);
    return devices();
}

std::size_t CEvtRec::devices()
{
    return _devList.size()-1; //exclude pipe
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

int CEvtRec::scanDir(const char *dirname)
{
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;

    printf("scan_dir start %s,%d\n",dirname);

    printf("dir(%s) opening...\n",dirname);
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    printf("dir(%s) opened\n",dirname);

    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';

    printf("filename=%s\n",filename);
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0'))) {
	    printf("continue,d_name:%d,%d,%d\n",de->d_name[0],de->d_name[1],de->d_name[1],de->d_name[2]);
            continue;
	}

        strcpy(filename, de->d_name);

        int no;
        sscanf(devname,DEVICE_FORMAT,&no);
        addDevice(no);
        printf("device(%s) opening...%d\n",devname,no);
    }
    closedir(dir);
    printf("dir(%s) closed\n",dirname);

    printf("scan_dir end %s\n",dirname);
    return 0;
}

bool CEvtRec::devOpen()
{
    scanDir(DEVICE_PATH);
    _errDev = -1;
    int i=0;
    char name[PATH_MAX];
    _POLLFD *pFds = new _POLLFD[_devList.size()];
    for(std::list<int>::iterator it=_devList.begin();it!=_devList.end();it++){
        pFds[i].events = POLLIN;

        if(*it!=-1){
            sprintf(name,DEVICE_FORMAT,*it);
            pFds[i].fd = open(name,O_RDONLY|O_NDELAY);
            if(pFds[i].fd<0) {
                delete [] pFds;
                _errDev = *it;
                return false;
            }
        }else{
            if(pipe(_fdPipe)<0) {
                delete [] pFds;
                _errDev = *it;
                return false;
            }
            pFds[i].fd = _fdPipe[0];
        }
        i++;
    }
    _pFds = pFds;
    return true;
}

void CEvtRec::devClose()
{
	int i=0;
    for(std::list<int>::iterator it=_devList.begin();it!=_devList.end();it++){
        if(*it!=-1){
			close(_pFds[i].fd);
        }else{
			close(_fdPipe[0]);
			close(_fdPipe[1]);
        }
		i++;
    }
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
                if(*it==-1) {
                    char q;
                    size = read(p->_pFds[i].fd,&q,sizeof(q));
                    printf("quit readEvent(%c)\n",q);
                    return (void*)REVT_OK;
                }else{
                    size = read(p->_pFds[i].fd,&event,sizeof(event));
                    if(size!=sizeof(event)) {
                        return (void*)REVT_ERR_READ; //read error
                    }
                    if(p->_pEvtDump)
                        if(p->_pEvtDump->evtDmp(*it,event)!=0)
                            return (void*)REVT_ERR_DUMP;
                }
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
    return write(_fdPipe[1],"Q",1);
}

int CEvtRec::wait()
{
    int status=0;
    return pthread_join(_id,(void**)&status);
}
