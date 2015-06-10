#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <list>
#include <string>
#include <linux/input.h>
#include <assert.h>
#include <stdlib.h>

static int scan_dir(const char *dirname,const char *prefix,std::list<std::string>& list)
{
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;

    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    printf("dir(%s) opened\n",dirname);

    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';

    char format[PATH_MAX];
    sprintf(format,"%s-%%d-%%d.bin",prefix);
    printf("comparing pattern=%s\n",format);
    while((de = readdir(dir))) {
        //printf("readdir, (%s)\n",de->d_name);
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0'))) {
	    //printf("continue,d_name:%d,%d,%d\n",de->d_name[0],de->d_name[1],de->d_name[1],de->d_name[2]);
            continue;
	}

        strcpy(filename, de->d_name);
        
        int order=-1,evtNo=-1,rtn=-1;
        rtn = sscanf(de->d_name,format,&order,&evtNo);
        if(rtn==2){
            list.push_back(std::string(de->d_name));
            printf("selected file(%s) pattern matched(order:%d,evtNo:%d,match:%d)\n",de->d_name,order,evtNo,rtn);
        }else{
            printf("selected file(%s) pattern not matched\n",de->d_name);
        }
    }
    closedir(dir);
    printf("dir(%s) closed\n",dirname);
    return 0;
}

int main(int argc, char *argv[])
{
    const char *path = ".";
    if(argc<4) {
        fprintf(stderr, "usage: %s [path of files] [file prefix] [sleep(sec)]\n", argv[0]);
        return 1;
    }

    int sec =  atoi(argv[3]);
    printf("path of files:%s\n",argv[1]);
    printf("file prefix:%s\n",argv[2]);
    printf("sleep(sec):%d\n",sec);
    
    std::list<std::string> files;
    path = argv[1];
    if(scan_dir(path,argv[2],files) < 0) {
        fprintf(stderr, "scan dir failed for %s\n", path);
        return 1;
    }
    printf("list contains %d elements\n",files.size());

    char format[PATH_MAX];
    sprintf(format,"%s-%%d-%%d.bin",argv[2]);
    for(std::list<std::string>::iterator it=files.begin();it!=files.end();it++) 
    {
        std::string fileName = std::string(path)+std::string("/")+(*it);
        printf("sending %s...\n",fileName.c_str());
        int fd;
        if((fd=open(fileName.c_str(),O_RDONLY)) < 0) {
            fprintf(stderr, "could not open %s, %s\n",fileName.c_str(),strerror(errno));
            return 1;
        }

        int order=-1,evtNo=-1,rtn=-1,fdw=-1;
        rtn = sscanf((*it).c_str(),format,&order,&evtNo);
        if(rtn!=2){
            fprintf(stderr, "error sscanf for evtNo %s\n",(*it).c_str());
            return 1;
        }else{
            assert(evtNo!=-1);
            char device[PATH_MAX];
            sprintf(device,"/dev/input/event%d",evtNo);
            if((fdw=open(device,O_WRONLY|O_SYNC)) < 0) {
                fprintf(stderr, "could not open %s, %s\n",device,strerror(errno));
                return 1;
            }
        }

        ssize_t size;
        struct input_event event; 
        while(1){
            size = read(fd,&event,sizeof(event)); 
            if(size!=(ssize_t)sizeof(event)) break;

            size = write(fdw,&event,sizeof(event));
            if(size!=(ssize_t)sizeof(event)){
                printf("write error(%d)\n",size);
                break;
            }
        }
        sleep(sec);
        //printf("sleep(%dsec)\n",sec);

        close(fdw);
        close(fd);
    }
    printf("end\n");
    return 0;
}
