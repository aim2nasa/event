#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

static int scan_dir(const char *dirname)
{
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;

    printf("scan_dir start %s\n",dirname);

    //printf("dir(%s) opening...\n",dirname);
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    printf("dir(%s) opened\n",dirname);

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
        printf("device(%s) opening...\n",devname);
        //open_device(devname);
    }
    closedir(dir);
    printf("dir(%s) closed\n",dirname);

    printf("scan_dir end %s\n",dirname);
    return 0;
}

int main(int argc, char *argv[])
{
    const char *path = ".";
    if(argc<3) {
        fprintf(stderr, "usage: %s [path of files] [file prefix]\n", argv[0]);
        return 1;
    }
    
    path = argv[1];
    if(scan_dir(path) < 0) {
        fprintf(stderr, "scan dir failed for %s\n", path);
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
