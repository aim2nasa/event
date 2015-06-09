#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd;
    ssize_t ret;
    int version;
    struct input_event event;

    if(argc<3) {
        fprintf(stderr, "use: %s [event binary file] [event#]\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if(fd < 0) {
        fprintf(stderr, "could not open %s, %s\n", argv[optind], strerror(errno));
        return 1;
    }

    int count=0;
    char dev[64];
    sprintf(dev,"/dev/input/event%d",atoi(argv[2]));
    printf("target device:%s\n",dev);
    int fdw = open(dev,O_CREAT|O_WRONLY);
    while(1){
        ret = read(fd, &event, sizeof(event));
        if(ret < (ssize_t) sizeof(event)) {
            printf("read done(%d)\n",ret);
            break; 
        }
        //printf("%d read\n",ret);

        ret = write(fdw,&event,sizeof(event));
        if(ret < (ssize_t) sizeof(event)) {
            printf("write error(%d)\n",ret);
            break;
        } 
        printf("events :%d written:%d bytes\n",++count,ret);
    }
    close(fdw);
    close(fd);
    printf("end\n");
    return 0;
}
