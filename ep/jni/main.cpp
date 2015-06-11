#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    printf("event play x1 version\n");
    if(argc<2) {
        printf("usage:ep <event dump file>\n");
        return -1;
    }

    int fd;
    if((fd=open(argv[1],O_RDONLY)) < 0) {
        fprintf(stderr, "could not open %s, %s\n",argv[1],strerror(errno));
        return -1;
    }

    close(fd);
    printf("end\n");
    return 0;
}
