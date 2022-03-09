#include <stdio.h>
#include <time.h> // clock_gettime
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "../commonheaders.h"

int main(int argc, char* argv[]) {
    char path[MAX_PATH_LENGTH];
    int fd;
    int write_times = 4;
    long long size = 1LL * 100 * 1024 * 1024;
    struct timespec start, end;
    double  accum;

    struct stat info;

    getServerPath("", argv[1], path);

    printf("filepath : %s\n", path);
    lstat(path, &info);

    printf("Last Mod: %ld\n", info.st_mtime);

    char *str = (char *) malloc(size + 1);
    memset(str, '*', size);
    str[size] = 0;
    for(int i = 0; i < write_times; i++) {
        
        fd = open(path, O_RDWR | O_APPEND | O_CREAT);

        if(fd < 0) {
            std::cout << "Error: File does not exist!" << std::endl;
        }
        clock_gettime( CLOCK_REALTIME, &start);
        write(fd, str, strlen(str)+1);
        
        fsync(fd);
        close(fd);
        clock_gettime( CLOCK_REALTIME, &end);
       

        accum = ((double)end.tv_sec * 1000 + 1.0e-6*end.tv_nsec) - 
           ((double)start.tv_sec * 1000 + 1.0e-6*start.tv_nsec);


      
        std::cout << "total time for the " << i+1 << "th time is: " << accum  << "ms"<<std::endl;
       
    }
    free(str);
    return 0;
}