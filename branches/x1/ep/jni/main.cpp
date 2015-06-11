#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <linux/input.h>

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

    close(fd);
    printf("end\n");
    return 0;
}
