#include "CEvtPlay.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <map>
#include <assert.h>

#define DEVICE_FORMAT "/dev/input/event%d"

CEvtPlay::CEvtPlay()
:_fd(-1),_fileSize(-1),_count(-1),_pEvtPlay(NULL)
{

}


CEvtPlay::~CEvtPlay()
{

}

int CEvtPlay::init(const char *file,IEvtPlay *p)
{
    struct stat fileInfo;
    if(stat(file,&fileInfo)==-1) return -1;

    int fd;
    if((fd=open(file,O_RDONLY)) < 0) return -2;

    _fd = fd;
    _fileSize = fileInfo.st_size;
    _count = _fileSize/(long long)(sizeof(struct input_event)+sizeof(int));
    _pEvtPlay = p;
    return 0;
}

long long CEvtPlay::fileSize()
{
    return _fileSize;
}

long long CEvtPlay::events()
{
    return _count;
}

int CEvtPlay::play(long long startLoc,long long endLoc)
{
    if((startLoc<0)||(endLoc<0)) return -100; 
    if(startLoc>endLoc) return -101; 
    if(endLoc>(_count-1)) return -102;
    long long maxRead = endLoc-startLoc+1;

    struct timeval tdiff;
    struct input_event event;

    timerclear(&tdiff);

    std::map<int,int> fdMap;    
    std::map<int,int>::iterator it;
    int device;

    lseek(_fd,(sizeof(device)+sizeof(event))*(startLoc),SEEK_CUR);
    for(int i=0; i<maxRead;i++)
    {
        struct timeval now, tevent, tsleep;

        if(read(_fd,&device,sizeof(device))!=sizeof(device)) {
	    return -1;
	}

        int deviceFd=-1;
        if((it=fdMap.find(device))!=fdMap.end()) {
            deviceFd = it->second;
        }else{
            char name[PATH_MAX];
            sprintf(name,DEVICE_FORMAT,device);
            deviceFd = open(name,O_WRONLY | O_NDELAY);
            if(deviceFd<0) {
                return -2;
            }
            fdMap.insert(std::pair<int,int>(device,deviceFd));
        }

	if(read(_fd,&event,sizeof(event))!=sizeof(event)) {
	    return -3;
	}

	gettimeofday(&now, NULL);
	if(!timerisset(&tdiff)) timersub(&now, &event.time, &tdiff); 

	timeradd(&event.time, &tdiff, &tevent);
	timersub(&tevent, &now, &tsleep);

	if(tsleep.tv_sec > 0 || tsleep.tv_usec > 100)
	    select(0, NULL, NULL, NULL, &tsleep);

	event.time = tevent;

        assert(deviceFd!=-1);
	if(write(deviceFd,&event,sizeof(event))!=sizeof(event)) {
	    return -4;
	}
        if(_pEvtPlay) 
            _pEvtPlay->play(i+1,device,event.time.tv_sec,event.time.tv_usec,
                               event.type,event.code,event.value);
    }
    for(it=fdMap.begin();it!=fdMap.end();++it) close(it->second);
    return 0;
}
