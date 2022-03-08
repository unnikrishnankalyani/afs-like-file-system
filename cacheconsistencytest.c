#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <dirent.h>

void client_w_1(char* path){
    int fd = open(path, O_CREAT | O_RDWR);
    lseek(fd, 0, SEEK_SET);
    write(fd, "client_w_1", 12);
    sleep(30); //run test2.c on Client 2
    close(fd);
    //run test2.c again on Client 2
}
void client_r_2(char* path){
    int fd = open(path, O_CREAT | O_RDWR);
    char* readbuf = (char*) malloc(12);
    read(fd, readbuf, 12);
    printf("read: %s\n",readbuf);
    close(fd);

}
void client_w_2(char* path){
    int fd = open(path, O_CREAT | O_RDWR);
    lseek(fd, 0, SEEK_SET);
    write(fd, "client_w_2", 12);
    close(fd);
}

void run_tests(char* path){
    client_w_1(path);
    client_w_2(path);
    client_r_2(path);
}

int main(int argc, char* argv[]){
    if (argc < 2){
        printf("Mount Path not provided.\n Usage: ./tester /mount-directory-path/\n");
        exit(1);
    }
    char* path = argv[1];
    int len = strlen(path);
    if(path[len -1] != '/'){
        path[len] = '/';
        path[len+1] = '\0';
    }
    run_tests(path);
}