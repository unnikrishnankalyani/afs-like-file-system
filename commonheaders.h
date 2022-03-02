#define MAX_PATH_LENGTH 1000
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

void getServerPath(const char* clientpath, const char*  root_path, char* server_path)
{
    server_path[0] = '\0';
    strncat(server_path, root_path, MAX_PATH_LENGTH);
    strncat(server_path, clientpath, MAX_PATH_LENGTH);
}