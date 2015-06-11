#include <stdio.h>
#include <stdlib.h>
#include <list>

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

    return 0;
}
