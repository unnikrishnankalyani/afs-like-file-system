#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "../commonheaders.h"

int main(int argc, char* argv[]) {
    char path[MAX_PATH_LENGTH];
    char content[100];
    int fd, fd1, fd2;
    int write_times = 7;

    struct stat info;
    char arg[100] = "../cmake/build/temp8/fuse/test4";
    getServerPath("", arg, path);

    printf("filepath : %s\n", path);
    lstat(path, &info);

    printf("Last Mod: %ld\n", info.st_mtime);
    for(int i == 0; i < write_times; i++) {
        // fd = open(path, O_RDWR | O_APPEND);

        // if(fd < 0) {
        //     std::cout << "Error: File does not exist!" << std::endl;
        // }
        auto start = std::chrono::system_clock::now();
        write(fd, "append", strlen("append")+1);
        fsync(fd);
        close(fd);
        auto end = std::chrono::system_clock::now();
        int writetime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        std::cout << "Write a file for the " << i+1 << "th time is: " << ((double) writetime) / 1e3  << "ms";
    }
    
    return 0;
}