#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "CEvtRec.h"
#include "CDump.h"

void sig_handler(int signo)
{
    printf("\nsignal(%d) received(%d)\n",signo);
    CEvtRec::instance()->stop();
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
        
    CEvtRec& er=*CEvtRec::instance();
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
    er.start();
    er.wait();
    printf("reading done\n");

    d.dumpClose();
    printf("end\n");
    return 0;
}
