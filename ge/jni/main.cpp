#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "CEvtRec.h"

class CDump : public IEvtDump{
public:
    CDump():_fd(-1){}
    virtual ~CDump(){}

    bool dumpOpen(const char *file)
    {
        int fd = open(file,O_WRONLY | O_CREAT | O_TRUNC,0644);
        if(fd<0) return false;
        _fd = fd;
        return true;
    } 

    int dumpClose() { return close(_fd); }

    int evtDmp(int dev,int type,int code,int value)
    {
        if(write(_fd,&dev,sizeof(int))!=sizeof(int)) return -1;
        if(write(_fd,&type,sizeof(int))!=sizeof(int)) return -2;
        if(write(_fd,&code,sizeof(int))!=sizeof(int)) return -3;
        if(write(_fd,&value,sizeof(int))!=sizeof(int)) return -4;
        printf("dev:%02d,type:%04x,code:%04x,value:%08x\n",dev,type,code,value);
        return 0;
    }

protected:
    int _fd;
};

void sig_handler(int signo)
{
  printf("\nsig_handler(%d)\n",signo);
}

int main(int argc, char *argv[])
{
    printf("get event x1 version\n");
    if(argc<3) {
        printf("usage:ge <dump file name> <event#> <event#>... <event#>\n");
        return -1;
    }
   
    CDump d;
    if(d.dumpOpen(argv[1]))
        printf("dump file:%s opened\n",argv[1]);
    else{
        printf("dump file:%s open failure\n",argv[1]);
        return -1;
    }   
        
    CEvtRec er;
    er.evtDump(&d);
    for(int i=3;i<=argc;i++) er.addDevice(atoi(argv[i-1]));

    printf("event# :");
    for(std::list<int>::iterator it=er.devList().begin();it!=er.devList().end();it++) 
        printf(" %d ",*it);
    printf("\n%d event# read in\n",er.devices()); 

    if(er.devOpen()) {
        printf("%d devices opened\n",er.devices());
    }else{
        printf("error in opening event#%d\n",er.errDev());
        return -1;
    }
 
    struct sigaction act;
    memset(&act,0,sizeof(act));
    act.sa_handler = sig_handler; 
    sigaction(SIGINT,&act,0);

    printf("reading for %d devices...\n",er.devices());
    int rtn;
    if((rtn=er.readEvent())!=0) //this function won't return until signal comes
        printf("error(%d) in readEvent()\n",rtn);

    d.dumpClose();
    printf("end\n");
    return 0;
}
