#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <linux/input.h>

int play(int fdRead,long long maxRead)
{
    struct timeval tdiff;
    struct input_event event;

    timerclear(&tdiff);
    
    int device;
    for(int i=0; i<maxRead;i++)
    {
        struct timeval now, tevent, tsleep;

        if(read(fdRead,&device,sizeof(device))!=sizeof(device)) {
	    fprintf(stderr,"can't read device number\n",strerror(errno));
	    return -1;
	}

	if(read(fdRead,&event,sizeof(event))!=sizeof(event)) {
	    fprintf(stderr,"can't read event\n",strerror(errno));
	    return -1;
	}

	gettimeofday(&now, NULL);
	if(!timerisset(&tdiff)) timersub(&now, &event.time, &tdiff); 

	timeradd(&event.time, &tdiff, &tevent);
	timersub(&tevent, &now, &tsleep);

	if(tsleep.tv_sec > 0 || tsleep.tv_usec > 100)
	    select(0, NULL, NULL, NULL, &tsleep);

	event.time = tevent;

/*
	if(write(out_fds[outputdev],&event,sizeof(event))!=sizeof(event)) {
	    fprintf(stderr,"can't write to device\n",strerror(errno));
	    return -1;
	}
*/
        printf("%d dev:%02d,time:%ld.%06ld,type:%04x,code:%04x,val:%08x\n", 
            i+1,device,event.time.tv_sec,event.time.tv_usec,event.type,event.code,event.value);
    }
}

int main(int argc, char *argv[])
{
    printf("event play x1 version\n");
    if(argc<2) {
        printf("usage:ep <event dump file>\n");
        return -1;
    }

    struct stat fileInfo;
    if(stat(argv[1],&fileInfo)==-1) {
        fprintf(stderr, "stat error %s, %s\n",argv[1],strerror(errno));
        return -1;
    }
    long long eventCount = fileInfo.st_size
        /(long long)(sizeof(struct input_event)+sizeof(int));
    printf("file size:%lldbytes,events:%lld\n",fileInfo.st_size,eventCount);  

    int fd;
    if((fd=open(argv[1],O_RDONLY)) < 0) {
        fprintf(stderr, "could not open %s, %s\n",argv[1],strerror(errno));
        return -1;
    }

    play(fd,eventCount);

    close(fd);
    printf("end\n");
    return 0;
}
