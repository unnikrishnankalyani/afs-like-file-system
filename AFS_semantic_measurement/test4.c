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

    struct stat info;
    char arg[100] = "../cmake/build/temp_yu/fuse/test4";
    getServerPath("", arg, path);

    printf("filepath : %s\n", path);
    lstat(path, &info);

    printf("Last Mod: %ld\n", info.st_mtime);

    fd = open(path, O_RDWR | O_APPEND);

    if(fd < 0) {
        std::cout << "Error: File does not exist!" << std::endl;
    }

    write(fd, "append", strlen("append")+1);
    fsync(fd);
    close(fd);
    return 0;
}