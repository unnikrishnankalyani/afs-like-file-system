#include <grpc++/grpc++.h>
#include "afs.grpc.pb.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using grpc::ClientReader;

using afs::AFS;
using afs::CreateReq;
using afs::CreateRes;
using afs::LsReq;
using afs::LsRes;
using afs::FetchRequest;
using afs::FetchReply;

class AfsClient {
    public:
        AfsClient(std::shared_ptr<Channel> channel) : stub_(AFS::NewStub(channel)) {}

    int afs_CREATE(const char* path) {
        CreateReq request;

        request.set_path(path);

        CreateRes reply;

        ClientContext context;

        Status status = stub_->afs_CREATE(&context, request, &reply);
        
        //add Retry
        if(status.ok()){
            return reply.ack();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }

    int afs_FETCH(const std::string& path, char **buf, int *size)
    {
        FetchRequest request;
        request.set_path(path);


        FetchReply *reply = new FetchReply();

        ClientContext context;

        Status status = stub_->afs_FETCH(&context, request, reply);

        if (status.ok()) {
                    std::cout << reply->buf() <<std::endl;
            *buf = (char *)(reply->buf()).data();
                    printf("%s\n", *buf);
            *size = reply->size();
            return 0;
        } else {
            return -1;
        }
    }

    int afs_OPEN(const char *path, struct fuse_file_info *file_info)
    {
            char *buf;
            int size;
            int rc;
            int fd;
            int isStale = 0;
            int isFetched = 0;
            char cacheFileName[80];
            struct stat cacheFileInfo;
            struct stat remoteFileInfo;
            char local_path[PATH_MAX];
            local_path[0] = '\0';
            char cbuf[] = "Check String";
            char nbuf[1000];

            snprintf(cacheFileName, 80, "%lu", hash((unsigned char *)path));

            strncat(local_path, fs_path, PATH_MAX);
            strncat(local_path, cacheFileName, PATH_MAX);
            printf("path: %s\n", local_path);

            fd = open(local_path,   O_APPEND | O_RDWR);

            if(fd == -1) {
                printf("Open Return: %d\n", fd);

                rc = afs_FETCH(path, &buf, &size);
                if (rc<0) {
                    return -ENOENT;
                }

                isFetched = 1;

                fd = creat(local_path, S_IRWXU);
                if(fd==-1) {
                    printf("Create Error\n");
                    return -errno;
                }
                fd = open(local_path,  O_APPEND | O_RDWR);
                if(fd==-1) printf("Reopen Error\n"); 
            } else {

                lstat(local_path, &cacheFileInfo);
                afs_GETATTR(path, &remoteFileInfo); 

                if(remoteFileInfo.st_mtime > cacheFileInfo.st_mtime) {
                    isStale = 1;
                }

                if(isStale) {
                    rc = ftruncate(fd, 0);
                    if(rc<0) {
                        return -errno;
                    }
                    rc = afs_FETCH(path, &buf, &size);
                    if (rc<0) {
                        return -ENOENT;
                    }
                    isFetched = 1;
                }
            }

            printf("File descr: %d Size:%d\n", fd, size);


            if(isFetched) {
                write(fd, buf, size);
                fsync(fd);
            }

            printf("File Contents: %s\n", buf);
        //        fi->fh_old = 0;
            fi->fh = fd; 

        return 0;
    }

    int afs_READ(const char *path, char *buffer, size_t size, off_t offset,
		      struct fuse_file_info *file_info){
        
        int ret_code = 0;

        ret_code = pread(file_info->fh, buffer, size, offset);
        if(ret_code < 0) {
            return -errno;
        }

	    return ret_code;
    }

    int afs_LS(const std::string& path, void *buf, fuse_fill_dir_t filler) {
        LsReq request;
        request.set_path(path);

        LsRes reply;

        ClientContext context;

        std::unique_ptr<ClientReader<LsRes> > reader(
            stub_->afs_LS(&context, request));

        while (reader->Read(&reply)) {
            if(reply.error()==0) {
                filler(buf, (reply.name()).c_str(), NULL, 0, static_cast<fuse_fill_dir_flags>(0));
            }
        }

        Status status = reader->Finish();

        if (status.ok()) {
            return 0;
        } else {
            return -1;
        }
}


    int afs_WRITE(const char *path, const char *buffer, size_t size, off_t offset,
                      struct fuse_file_info *file_info, char fs_path[]){
        int ret_code = 0;
        struct stat info;

        // printf("File closed: %d\n", fcntl(fi->fh, F_GETFD));
        // printf("File closed err: %d\n", errno);
        // printf("Write File descriptor: %d\n", fi->fh);
        ret_code = write(file_info->fh, buffer, size);
        fstat(file_info->fh, &info);
        // printf("Write return: %d\n", info.st_mtime);
        if(ret_code < 0) {
            printf("Error while writing into file: %d\n", errno);
            int fd;
            char cached_file[80];
            char local_path[PATH_MAX];
            local_path[0] = '\0';

            // snprintf(cached_file, 80, "%lu", hash((unsigned char *)path));
            strncat(local_path, fs_path, PATH_MAX);
            strncat(local_path, cached_file, PATH_MAX);

            fd = open(local_path,  O_APPEND | O_RDWR);

            // printf("Newdile fd: %d\n", fd);
            lseek(fd,offset,SEEK_SET);
            for(int i=0; i<size; i++) {
                printf("%c", buffer[i]);
            }
            ret_code = write(fd, buffer, size);
            close(fd);
            if(ret_code<0) {
                printf("Error while re-writing file %d\n", errno);
                printf("Return error: %d\n", ret_code);
                return -errno;
            }
        }
        return ret_code;
    }

    private:
        std::unique_ptr<AFS::Stub> stub_;
};