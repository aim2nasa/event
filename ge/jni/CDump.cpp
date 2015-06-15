#include "CDump.h"
#include <fcntl.h>
#include <stdio.h>
#include <linux/input.h>

CDump::CDump()
:_fd(-1)
{

}

CDump::~CDump()
{

}

bool CDump::dumpOpen(const char *file)
{
    int fd = open(file,O_WRONLY | O_CREAT | O_TRUNC,0644);
    if(fd<0) return false;
    _fd = fd;
    return true;
} 

int CDump::dumpClose()
{
    return close(_fd);
}

int CDump::evtDmp(int dev,struct input_event& event)
{
    if(write(_fd,&dev,sizeof(dev))!=sizeof(dev)) return -1;
    if(write(_fd,&event,sizeof(event))!=sizeof(event)) return -2;
    printf("dev:%02d,type:%04x,code:%04x,value:%08x\n",dev,event.type,event.code,event.value);
    return 0;
}
