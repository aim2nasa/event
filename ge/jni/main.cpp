#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <fcntl.h>
#include <errno.h>

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
    printf("\n"); 

    int fdw = open(argv[1],O_WRONLY | O_CREAT | O_TRUNC);
    if(fdw<0){
        fprintf(stderr, "could not open %s, %s\n",argv[1],strerror(errno));
        return -1;
    }
    close(fdw);
    return 0;
}
