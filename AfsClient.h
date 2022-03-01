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

    int afs_READ(const char *path, char *buffer, size_t size, off_t offset,
		      struct fuse_file_info *file_info){
        
        int ret_code = 0;

        ret_code = pread(file_info->fh, buffer, size, offset);
        if(ret_code < 0) {
            return -errno;
        }

	    return ret_code;
    }

    int AFS::ListDir(const std::string& path, void *buf, fuse_fill_dir_t filler) {
        ListDirRequest request;
        request.set_path(path);

        ListDirReply reply;

        ClientContext context;

        std::unique_ptr<ClientReader<ListDirReply> > reader(
            stub_->ListDir(&context, request));

        while (reader->Read(&reply)) {
            if(reply.error()==0) {
                filler(buf, (reply.name()).c_str(), NULL, 0);
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