#include <stdio.h>
#include <time.h> // clock_gettime
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "../commonheaders.h"

int main(int argc, char* argv[]) {
    char path[MAX_PATH_LENGTH];
    char content[100];
    int fd, fd1, fd2;
    int write_times = 4;
    long long size = 1LL * 100 * 1024 * 1024;
    struct timespec start, end, medium1, medium2, medium0;
    double accum0, accum1, accum2, accum3, accum4;

    struct stat info;

    getServerPath("", argv[1], path);

    printf("filepath : %s\n", path);
    lstat(path, &info);

    printf("Last Mod: %ld\n", info.st_mtime);

    char *str = (char *) malloc(size + 1);
    memset(str, '*', size);
    str[size] = 0;
    for(int i = 0; i < write_times; i++) {
        clock_gettime( CLOCK_REALTIME, &start);
        fd = open(path, O_RDWR | O_APPEND | O_CREAT);

        if(fd < 0) {
            std::cout << "Error: File does not exist!" << std::endl;
        }
        clock_gettime( CLOCK_REALTIME, &medium0);
        write(fd, str, strlen(str)+1);
        clock_gettime( CLOCK_REALTIME, &medium1);
        fsync(fd);
        clock_gettime( CLOCK_REALTIME, &medium2);
        close(fd);
        clock_gettime( CLOCK_REALTIME, &end);
        accum0 = ((double)medium0.tv_sec * 1000 + 1.0e-6*medium0.tv_nsec) - 
           ((double)start.tv_sec * 1000 + 1.0e-6*start.tv_nsec);
        accum1 = ((double)medium1.tv_sec * 1000 + 1.0e-6*medium1.tv_nsec) - 
           ((double)medium0.tv_sec * 1000 + 1.0e-6*medium0.tv_nsec);
        accum2 = ((double)medium2.tv_sec * 1000 + 1.0e-6*medium2.tv_nsec) - 
           ((double)medium1.tv_sec * 1000 + 1.0e-6*medium1.tv_nsec);
        accum3 = ((double)end.tv_sec * 1000 + 1.0e-6*end.tv_nsec) - 
           ((double)medium2.tv_sec * 1000 + 1.0e-6*medium2.tv_nsec);

        accum4 = ((double)end.tv_sec * 1000 + 1.0e-6*end.tv_nsec) - 
           ((double)medium0.tv_sec * 1000 + 1.0e-6*medium0.tv_nsec);


      //   std::cout << "open file for the " << i+1 << "th time is: " << accum0  << "ms"<<std::endl;
      //   std::cout << "write file for the " << i+1 << "th time is: " << accum1  << "ms"<<std::endl;
      //   std::cout << "fsync file for the " << i+1 << "th time is: " << accum2  << "ms"<<std::endl;
      //   std::cout << "close file for the " << i+1 << "th time is: " << accum3  << "ms"<<std::endl;
        std::cout << "total time for the " << i+1 << "th time is: " << accum4  << "ms"<<std::endl;
       
    }
    free(str);
    return 0;
}