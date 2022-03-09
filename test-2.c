#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "commonheaders.h"
// test create 
int main(int argc, char* argv[]) {
    char path[MAX_PATH_LENGTH];
    
    int fd;

    struct stat info;
    getLocalPath("", argv[1],path);
    printf("filepath : %s\n", path);
    lstat(path, &info);
    printf("Last Mod: %ld\n", info.st_mtime);

    creat(path, O_RDWR | O_APPEND |O_CREAT);
    fd = open(path, O_RDWR | O_APPEND);

    if(fd<0) {
        printf("creation failed\n");
    }

    int size;

    if (NULL != fd) {
        fseek (fd, 0, SEEK_END);
        size = ftell(fd);

        if (0 == size) {
            printf("file is empty\n");
        }
    }
    return 0;
}