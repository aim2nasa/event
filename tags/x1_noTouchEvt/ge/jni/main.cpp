#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <linux/input.h>

#define DEVICE_FORMAT "/dev/input/event%d"

typedef pollfd POLLFD;

POLLFD* init(std::list<int>& list)
{
    int i=0;
    char name[PATH_MAX]; 
    POLLFD *pFds = new POLLFD[list.size()];
    for(std::list<int>::iterator it=list.begin();it!=list.end();it++){
        sprintf(name,DEVICE_FORMAT,*it);
        pFds[i].events = POLLIN;
        pFds[i].fd = open(name,O_RDONLY|O_NDELAY);
        if(pFds[i].fd<0) {
            delete [] pFds;
            fprintf(stderr,"could not open %s, %s\n",name,strerror(errno));
            return NULL;
        }
        printf("device %s opened\n",name);
        i++;
    }
    return pFds;
}

int dump(int fd,int device,struct input_event& event)
{
    if(write(fd,&device,sizeof(device))!=sizeof(device)){
        fprintf(stderr,"device,write error to device(%d),%s\n",device,strerror(errno));
        return -1;
    }
    if(write(fd,&event,sizeof(event))!=sizeof(event)){
        fprintf(stderr,"event,write error to device(%d),%s\n",device,strerror(errno));
        return -1;
    }
    printf("device:%02d,type:0x%04x,code:0x%04x,value:0x%08x,%dbytes\n",
        device,event.type,event.code,event.value,sizeof(device)+sizeof(event));
    return 0;
}
 
int record(POLLFD* pFds,std::list<int>& list,int fdOut)
{
    struct input_event event;
    ssize_t size;
    while(1)
    {
        if(poll(pFds,list.size(),-1) <0) {
            printf("poll failed\n");
            return -1;
        }

        std::list<int>::iterator it=list.begin();
        for(int i=0;i<list.size();i++) {
            if(pFds[i].revents & POLLIN) {
                size = read(pFds[i].fd,&event,sizeof(event));
                if(size!=sizeof(event)) {
                    printf("read error(%d)\n",size);
                    return -1;
                }
		if(dump(fdOut,*it,event)!=0) return -1;
            }
            it++;
        }
    }
}

int main(int argc, char *argv[])
{
    printf("get event x1 version\n");
    if(argc<3) {
        printf("usage:ge <dump file name> <event#> <event#>... <event#>\n");
        return -1;
    }

    std::list<int> devList;
    for(int i=3;i<=argc;i++) devList.push_back(atoi(argv[i-1]));
    printf("event# :");
    for(std::list<int>::iterator it=devList.begin();it!=devList.end();it++) 
        printf(" %d ",*it);
    printf("\n%d events registered\n",devList.size()); 

    POLLFD *in_fds = init(devList);
    if(in_fds==NULL){
        fprintf(stderr, "init failed\n");
        return -1;
    }

    int fdw = open(argv[1],O_WRONLY | O_CREAT | O_TRUNC,0644);
    if(fdw<0){
        fprintf(stderr, "could not open %s, %s\n",argv[1],strerror(errno));
        return -1;
    }

    printf("recording for %d devices...\n",devList.size());
    record(in_fds,devList,fdw);

    delete [] in_fds;
    close(fdw);
    printf("end\n");
    return 0;
}
