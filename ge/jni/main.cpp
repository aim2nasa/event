#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <linux/input.h>
#include "CEvtRec.h"

class CMsg : public IEvtDump{
public:
    int evtDmp(int dev,int type,int code,int value)
    {
        printf("dev:%02d,type:%04x,code:%04x,value:%08x\n",dev,type,code,value);
    }
};

int main(int argc, char *argv[])
{
    printf("get event x1 version\n");
    if(argc<3) {
        printf("usage:ge <dump file name> <event#> <event#>... <event#>\n");
        return -1;
    }
   
    CMsg m;
    CEvtRec er;
    er.evtDump(&m);
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
 
    if(!er.recOpen(argv[1])) {
        printf("error in opening %s\n",argv[1]);
        return -1;
    }

    printf("recording for %d devices...\n",er.devices());
    er.rec();

    er.recClose();
    printf("end\n");
    return 0;
}
