#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

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

    int fdw = open(argv[1],O_WRONLY | O_CREAT | O_TRUNC);
    if(fdw<0){
        fprintf(stderr, "could not open %s, %s\n",argv[1],strerror(errno));
        return -1;
    }

    delete [] in_fds;
    close(fdw);
    return 0;
}
