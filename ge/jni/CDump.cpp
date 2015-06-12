#include "CDump.h"
#include <fcntl.h>
#include <stdio.h>

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

int CDump::evtDmp(int dev,int type,int code,int value)
{
    if(write(_fd,&dev,sizeof(int))!=sizeof(int)) return -1;
    if(write(_fd,&type,sizeof(int))!=sizeof(int)) return -2;
    if(write(_fd,&code,sizeof(int))!=sizeof(int)) return -3;
    if(write(_fd,&value,sizeof(int))!=sizeof(int)) return -4;
    printf("dev:%02d,type:%04x,code:%04x,value:%08x\n",dev,type,code,value);
    return 0;
}
