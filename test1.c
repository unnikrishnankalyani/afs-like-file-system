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



void testCreateAndCheckExists(char* path){
    int len = strlen(path);
    char path1[1024];
    strncpy(path1, path,len);
    path1[len] = '\0';
    strcat(path1, "test1.txt");
    //printf("File path: %s\n", path1);
    int fd = creat(path1, 0777);
    int res = access(path1, F_OK);
    if(res == 0){
        printf("Test 1 Create a file : PASSED\n");
    }
    else{
        printf("Test 1 Create a file: FAILED\n");
    close(fd);
    remove(path1);
}
}


void testCreateWriteAndRead(char* path){
    int len = strlen(path);
    char path2[1024];
    strncpy(path2, path,len);
    path2[len] = '\0';
    strcat(path2, "test2.txt");
    int fd = open(path2, O_CREAT | O_RDWR);
    char* writebuf = "test2";
    
    lseek(fd, 0, SEEK_SET);
    write(fd, writebuf, 5);
    fsync(fd);
    close(fd);
    fd = open(path2, O_RDONLY);
    char* readbuf = (char*) malloc(5);
    read(fd,readbuf, 5);
    if (strcmp(writebuf, readbuf) == 0){
        printf("Test 2 Write and Read : PASSED\n");
    }
    else{
        printf("Test 2 Write and Read : FAILED\n");
    }
    close(fd);
    remove(path2);
    free(readbuf);
}

void testCreateAndDelete(char* path){
    int len = strlen(path);
    char path3[1024];
    strncpy(path3, path,len);
    path3[len] = '\0';
    strcat(path3, "test3.txt");

    int fd = creat(path3, 0777);
    close(fd);
    remove(path3);
    int res = access(path3, F_OK);
    if(res != -1){
        printf("Test 3 Delete a file : FAILED\n");
    }
    else{
        printf("Test 3 Delete a file: PASSED\n");
    }
}

void testCreateDirectory(char* path){
    int len = strlen(path);
    char path4[1024];
    strncpy(path4, path,len);
    path4[len] = '\0';
    strcat(path4, "test4");
    int res = mkdir(path4,0777);
    DIR* dir = opendir(path4);
   if (dir){
        printf("Test 4 Create a directory: PASSED\n");
    }
    else{
        printf("Test 4 Create a directory: FAILED\n");
    }
    rmdir(path4); 
}

void testDeleteDirectory(char* path){
    int len = strlen(path);
    char path5[1024];
    strncpy(path5, path,len);
    path5[len] = '\0';
    strcat(path5, "test5");
    int res = mkdir(path5,0777);
    rmdir(path5);
    DIR* dir = opendir(path5);
    if (ENOENT == errno){
        printf("Test 5 Delete Directory: PASSED\n");
    }
    else{
        printf("Test 5 Delete Directory: FAILED\n");
    }
}

void testCreateNestedDirectoriesAndFiles(char* path){
    
}

void testDeleteNestedDirectoriesAndFiles(char* path){

}

void testAppend(char *path){
    int len = strlen(path);
    char path6[1024];
    strncpy(path6, path,len);
    path6[len] = '\0';
    strcat(path6, "test6.txt");
    int fd = open(path6, O_CREAT | O_RDWR);
    char* writebuf = "test6";
    char* readbuf = (char*) malloc(10);
    lseek(fd, 0, SEEK_SET);
    write(fd, writebuf, 5);
    fsync(fd);
    close(fd);
    fd = open(path6, O_RDWR);
    pwrite(fd, writebuf, 5, 5);
    fsync(fd);
    close(fd);
    fd = open(path6, O_RDONLY);
    read(fd,readbuf, 10);
    if (strcmp("test6test6", readbuf) == 0){
        printf("Test 6 Append : PASSED\n");
    }
    else{
        printf("Test 6 Append : FAILED\n");
    }

    close(fd);
    remove(path6);
    free(readbuf);
}



void testRandomRead(char* path){
    int len = strlen(path);
    char path7[1024];
    strncpy(path7, path,len);
    path7[len] = '\0';
    strcat(path7, "test7.txt");
    int fd = open(path7, O_CREAT | O_RDWR);
    char* writebuf = "xxxxxtest7xxxxx";
    char* readbuf = (char*) malloc(5);
    lseek(fd, 0, SEEK_SET);
    write(fd, writebuf, 15);
    fsync(fd);
    close(fd);
    fd = open(path7, O_RDWR);
    pread(fd, readbuf, 5, 5);
    if (strcmp("test7", readbuf) == 0){
        printf("Test 7 Random Read : PASSED\n");
    }
    else{
        printf("Test 7 Random Read : FAILED\n");
    }
    
    close(fd);
    remove(path7);
    free(readbuf);
}

void testRandomWrite(char* path){
    int len = strlen(path);
    char path8[1024];
    strncpy(path8, path,len);
    path8[len] = '\0';
    strcat(path8, "test8.txt");
    int fd = open(path8, O_CREAT | O_RDWR);
    char* writebuf = "test8xxxxxtest8";
    char* pwritebuf = "test8";
    write(fd, writebuf, 15);
    fsync(fd);
    close(fd);
    char* readbuf = (char*) malloc(15);
    fd = open(path8, O_RDWR);
    pwrite(fd, pwritebuf, 5, 5);
    fsync(fd);
    close(fd);
    fd = open(path8, O_RDONLY);
    read(fd, readbuf, 15);
    if (strcmp("test8test8test8", readbuf) == 0){
        printf("Test 8 Random Write : PASSED\n");
    }
    else{
        printf("Test 8 Random Write : FAILED\n");
    }
   
    close(fd);
    remove(path8);
    free(readbuf);
}



void run_tests(char* path){
    testCreateAndCheckExists(path);
    testCreateWriteAndRead(path);
    testCreateAndDelete(path);
    testCreateDirectory(path);
    testDeleteDirectory(path);
    //testCreateNestedDirectoriesAndFiles(path);
    //testDeleteNestedDirectoriesAndFiles(path);
    testAppend(path);
    testRandomRead(path);
    testRandomWrite(path);
}

int main(int argc, char* argv[]){
    if (argc < 2){
        printf("Mount Path not provided.\n Usage: ./tester /mount-directory-path/\n");
        exit(1);
    }
    char* mount_path = argv[1];
    int len = strlen(mount_path);
    if(mount_path[len -1] != '/'){
        mount_path[len] = '/';
        mount_path[len+1] = '\0';
    }
    run_tests(mount_path);
}