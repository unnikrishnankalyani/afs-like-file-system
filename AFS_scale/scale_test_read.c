#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "../commonheaders.h"
#include <time.h> // clock_gettime
#include <stdlib.h> // malloc

int main(int argc, char* argv[]) {
    char path[MAX_PATH_LENGTH];
    char content[100];
    int fd, fd1, fd2;

    struct stat info;
    getServerPath("", argv[1], path);

    // printf("filepath : %s\n", path);
    lstat(path, &info);

    struct timespec* t = (struct timespec*)malloc(sizeof(struct timespec)), * u = (struct timespec*)malloc(sizeof(struct timespec));
    long elapsed_time;

    clock_gettime(CLOCK_MONOTONIC, t);
    fd = open(path, O_RDWR | O_APPEND);

    if(fd < 0) {
        std::cout << "Error: File does not exist!" << std::endl;
    }
    close(fd);
    clock_gettime(CLOCK_MONOTONIC, u);
    elapsed_time = u->tv_nsec - t->tv_nsec;

    printf("******************************\n");
    printf("Time Diff: %2ld ns\n\n", elapsed_time);

    return 0;
}
