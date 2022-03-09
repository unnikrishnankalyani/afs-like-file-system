#include<stdio.h>
#include <time.h> // clock_gettime
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "../commonheaders.h"

int main(int argc, char* argv[]) {
    char path[MAX_PATH_LENGTH];
    char content[100];
    int fd;
    int open_times = 1;
    struct timespec start, end;
    double accum;

    struct stat info;
    getServerPath("", argv[1], path);

    printf("filepath : %s\n", path);
    lstat(path, &info);

    printf("Last Mod: %ld\n", info.st_mtime);
    for(int i = 0; i < open_times; i++) {
        clock_gettime( CLOCK_REALTIME, &start);
        fd = open(path, O_RDWR | O_APPEND | O_CREAT);
        clock_gettime( CLOCK_REALTIME, &end);
        accum = ((double)end.tv_sec * 1000 + 1.0e-6*end.tv_nsec) - 
           ((double)start.tv_sec * 1000 + 1.0e-6*start.tv_nsec);

        std::cout << "Access file for the " << i+1 << "th time is: " << accum  << "ms"<<
           std::endl;
        if(fd < 0) {
            std::cout << "Error: File does not exist!" << std::endl;
        }
    }
    

    return 0;
}