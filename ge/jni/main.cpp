#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>
#include <sys/limits.h>
#include <sys/poll.h>
#include <linux/input.h> // this does not compile
#include <errno.h>
#include <list>

#define MAX_DEVICES 128
#define DEVICE_FORMAT "/dev/input/event%d"

static struct pollfd *ufds;
static char **device_names;
static int nfds;

static int open_device(const char *device)
{
    //printf("open_device start %s\n",device);

    int fd;
    struct pollfd *new_ufds;
    char **new_device_names;

    fd = open(device, O_RDWR);
    if(fd < 0) {
        fprintf(stderr, "could not open %s, %s\n", device, strerror(errno));
        return -1;
    }
    printf("%s opened\n",device);

    new_ufds = (pollfd*)realloc(ufds, sizeof(ufds[0]) * (nfds + 1));
    if(new_ufds == NULL) {
        fprintf(stderr, "out of memory\n");
        return -1;
    }
    ufds = new_ufds;
    //printf("mem alloc for ufds(sizeof(ufds[0]):%d x %d)\n",sizeof(ufds[0]),nfds+1);

    new_device_names = (char**)realloc(device_names, sizeof(device_names[0]) * (nfds + 1));
    if(new_device_names == NULL) {
        fprintf(stderr, "out of memory\n");
        return -1;
    }
    device_names = new_device_names;
    //printf("mem alloc for dev names(sizeof(device_names[0]):%d x %d)\n",sizeof(device_names[0]),nfds+1);

    ufds[nfds].fd = fd;
    ufds[nfds].events = POLLIN;
    device_names[nfds] = strdup(device);
    nfds++;
    printf("open_device end nfds:%d fd:%d device:%s\n",nfds,fd,device);
    return 0;
}

int close_device(const char *device)
{
    int i;
    for(i = 1; i < nfds; i++) {
        if(strcmp(device_names[i], device) == 0) {
            int count = nfds - i - 1;
            printf("remove device %d: %s\n", i, device);
            free(device_names[i]);
            memmove(device_names + i, device_names + i + 1, sizeof(device_names[0]) * count);
            memmove(ufds + i, ufds + i + 1, sizeof(ufds[0]) * count);
            nfds--;
            return 0;
        }
    }
    fprintf(stderr, "remote device: %s not found\n", device);
    return -1;
}

static int scan_dir(const char *dirname)
{
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;

    //printf("scan_dir start %s,%d\n",dirname);

    //printf("dir(%s) opening...\n",dirname);
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    //printf("dir(%s) opened\n",dirname);

    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';

    //printf("filename=%s\n",filename);
    while((de = readdir(dir))) {
        //printf("readdir, (%s)\n",de->d_name);
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0'))) {
	    //printf("continue,d_name:%d,%d,%d\n",de->d_name[0],de->d_name[1],de->d_name[1],de->d_name[2]);
            continue;
	}

        strcpy(filename, de->d_name);
        //printf("device(%s) opening...\n",devname);
        open_device(devname);
    }
    closedir(dir);
    //printf("dir(%s) closed\n",dirname);

    printf("scan_dir end %s\n",dirname);
    return 0;
}

bool isWatchTarget(const char* devName,std::list<int>& l)
{
    //printf("\tdevice:%s ",devName);
    char tmp[PATH_MAX];
    for(std::list<int>::iterator it=l.begin();it!=l.end();it++){
        sprintf(tmp,DEVICE_FORMAT,*it);
        if(strcmp(devName,tmp)==0) {
            //printf(",found:%s\n",tmp);
            return true; 
        }
    }
    //printf(",not found\n");
    return false;
}

int main(int argc, char *argv[])
{
    int res;
    int pollres;
    struct input_event event;
    const char *device = NULL;
    const char *device_path = "/dev/input";

    if(argc<3) {
        printf("usage:ge <dump file name> <touch event#> <other event#>... <other event#>\n");
        printf("Note: touch event# MUST be in the first\n");
        return -1;
    }

    int touchEvtNo = atoi(argv[2]);
    printf("touch event#:%d\n",touchEvtNo);

    std::list<int> devList;
    for(int i=3;i<=argc;i++) devList.push_back(atoi(argv[i-1]));
    printf("event# :");
    for(std::list<int>::iterator it=devList.begin();it!=devList.end();it++) printf(" %d ",*it);
    printf("\n");

    nfds = 1;
    ufds = (pollfd*)calloc(1, sizeof(ufds[0]));
    ufds[0].fd = inotify_init();
    ufds[0].events = POLLIN;

    printf("device is NULL, add watch %s\n",device_path);
    res = inotify_add_watch(ufds[0].fd, device_path, IN_DELETE | IN_CREATE);
    if(res < 0) {
        fprintf(stderr, "could not add watch for %s, %s\n", device_path, strerror(errno));
        return 1;
    }
    printf("scanning dir %s\n",device_path);
    res = scan_dir(device_path);
    if(res < 0) {
        fprintf(stderr, "scan dir failed for %s\n", device_path);
        return 1;
    }

    int evtGroup = 0,mtTrack=0,evtCount=0;
    bool isLastMt = false,onMtTracking=false,isKey=false;
    char evtGroupName[MAX_DEVICES][PATH_MAX];
    int fd[MAX_DEVICES];
    memset(&fd,-1,sizeof(fd));

    bool fileOpen[MAX_DEVICES]; //big enough I think
    memset(&fileOpen,0,sizeof(fileOpen)); //When file is opened, must be true

    char selected[PATH_MAX];
    printf("entering while loop...\n");
    while(1) {
        pollres = poll(ufds, nfds, -1);
        //printf("poll %d, returned %d\n", nfds, pollres);
        for(int i = 1; i < nfds; i++) {
            if(ufds[i].revents) {
                if(ufds[i].revents & POLLIN) {
		    //printf("\tufds[%d].revents=%d, POLLIN ",i,ufds[i].revents);
                    res = read(ufds[i].fd, &event, sizeof(event));
                    if(res < (int)sizeof(event)) {
                        fprintf(stderr, ",could not get event\n");
                        return 1;
                    }
                    if(!isWatchTarget(device_names[i],devList)) continue;

                    int eventNo;
                    sscanf(device_names[i],DEVICE_FORMAT,&eventNo);
                    //printf("device name %s, extracted event no:%d\n",device_names[i],eventNo);
                    if(fileOpen[i]==false){
                        sprintf(evtGroupName[i],"%s-%d-%d.bin",argv[1],evtGroup++,eventNo);
                        fd[i] = open(evtGroupName[i], O_CREAT|O_WRONLY,0644);
                        if(fd[i] < 0) {
                            fprintf(stderr, "could not open %s, %s\n",evtGroupName[i],strerror(errno));
                            return -1;
                        } 
                        fileOpen[i]=true;
                        printf("%s file opened(fd[%d]=%d)\n",evtGroupName[i],i,fd[i]);
                    } 

                    int nWritten = write(fd[i],&event,sizeof(event));
                    if(nWritten!=sizeof(event)){
                        fprintf(stderr, "write error(%dbytes,fd[%d]=%d)\n",nWritten,i,fd);
                        return -1;
                    }
                    evtCount++;
                    printf("%d bytes %ld-%ld: %s %04x %04x %08x\n",
			nWritten,event.time.tv_sec, event.time.tv_usec,device_names[i],event.type, event.code, event.value);

                    //ABS_MT_TRACKING_ID 0x39
                    if(event.type==EV_ABS && event.code==0x39){
                        if(event.value!=0xffffffff) {
			    if(mtTrack==0) onMtTracking = true;
                            mtTrack++;
                            printf("MT Tracking(%d) started(%d)\n",mtTrack,event.value);
                        }else{
                            mtTrack--;
                            if(mtTrack==0) {
                                isLastMt = true;
			        onMtTracking = false;
                            }
                            printf("MT Tracking(%d) ended(%d)\n",mtTrack,event.value);
                        }
                    }

                    //EV_KEY
                    if(event.type==EV_KEY){
                        isKey = true;
                        printf("key(code:%d value:%d)\n",event.code,event.value);
                    }

                    //SYN_REPORT 0x0
                    if(event.type==EV_SYN && event.code==0x0 &&
                        (isLastMt==true||((touchEvtNo!=eventNo)&&isKey==true))){
                        close(fd[i]);
                        printf("%s file closed(fd[%d]=%d)\n",evtGroupName[i],i,fd[i]);
                        fileOpen[i]=false;
                        printf("End of event group(events:%d)\n",evtCount);
                        evtCount = 0;
                        if(isLastMt) isLastMt = false;
                        if(isKey) isKey = false;
                    }
                }
            }
        }
    }
    return 0;
}
