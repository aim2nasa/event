#include <stdio.h>
#include <stdlib.h>
#include "CEvtPlay.h"

class CMon : public IEvtPlay{
public:
    virtual ~CMon(){}

    int play(int order,int dev,long sec,long uSec,int type,int code,int value)
    {
        printf("%d dev:%02d,time:%ld.%06ld,type:%04x,code:%04x,val:%08x\n", 
            order,dev,sec,uSec,type,code,value);
        return 0;
    }
};

int main(int argc, char *argv[])
{
    printf("event play x1 version\n");
    if(argc<2) {
        printf("usage:ep <event dump file> Option(<startLoc> <endLoc>)\n");
        return -1;
    }
    
    CMon m;
    CEvtPlay ep;

    int rtn = -1;
    if((rtn=ep.init(argv[1],&m))!=0){
        fprintf(stderr, "init error(%d) %s\n",rtn,argv[1]);
        return -1;
    }
    printf("initialize ok with %s\n",argv[1]);
    printf("file size:%lldbytes,events:%lld\n",ep.fileSize(),ep.events());  

    if(argc>3)
        rtn = ep.play(atoll(argv[2]),atoll(argv[3]));
    else if(argc==3)
        rtn = ep.play(atoll(argv[2]),ep.events()-1);
    else 
        rtn = ep.play(0,ep.events()-1);

    printf("play returns(%d)\n",rtn);
    printf("end\n");
    return 0;
}
