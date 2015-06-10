#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if(argc<2) {
        fprintf(stderr, "usage: %s [dump file name]\n", argv[0]);
        return 1;
    }

    int fd;
    while(1)
    {
        if((fd=open(argv[1],O_RDONLY)) < 0) {
            fprintf(stderr, "could not open %s, %s\n", argv[1], strerror(errno));
            return 1;
        }

        close(fd);
    }
    printf("end\n");
    return 0;
}
