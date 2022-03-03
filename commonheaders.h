#define MAX_PATH_LENGTH 1000
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>

void getServerPath(const char* clientpath, const char*  root_path, char* server_path)
{
    server_path[0] = '\0';
    strncat(server_path, root_path, MAX_PATH_LENGTH);
    strncat(server_path, clientpath, MAX_PATH_LENGTH);
}

void getLocalPath(const char* file_path, const char*  cache_path, char* client_path)
{
    // char cacheFileName[80];
    // snprintf(cacheFileName, 80, "%lu", hash((unsigned char *)file_path));
    client_path[0] = '\0';
    strncat(client_path, cache_path, MAX_PATH_LENGTH);
    // strncat(client_path, cacheFileName, MAX_PATH_LENGTH);
    strncat(client_path, file_path, MAX_PATH_LENGTH);

}

unsigned long hash(unsigned char *str)
{   
    unsigned long hash = 5381;
    int c;
    
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    
    return hash;
}