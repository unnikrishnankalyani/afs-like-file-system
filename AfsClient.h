#include <grpc++/grpc++.h>
#include "afs.grpc.pb.h"
#include "commonheaders.h"

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
using afs::GetattrReq;
using afs::GetattrRes;
using afs::StoreReq;
using afs::StoreRes;

class AfsClient {
    public:
        AfsClient(std::shared_ptr<Channel> channel) : stub_(AFS::NewStub(channel)) {}

    int afs_CREATE(const char* path, char cache_path[]) {
        // CreateReq request;

        // request.set_path(path);

        // CreateRes reply;

        // ClientContext context;

        // Status status = stub_->afs_CREATE(&context, request, &reply);
        
        // //add Retry
        // if(status.ok()){
        //     return reply.ack();
        // } else {
        //     std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        //     return -1;
        // }
        int fd;

        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);
    
        printf("path: %s\n", client_path);
        fflush(stdout);
        fd = open(client_path, O_CREAT | O_APPEND | O_RDWR, mode );
        printf("Creating file in local cache\n");
        if (fd == -1) {
                printf("Create Error in local cache.. \n");
                return -errno;
        }

        fi->fh = fd;

        afs_Store(path, NULL, 0);

        printf("Create file descr: %d\n", fi->fh);
        return 0;
    }

    int afs_MKDIR(const char *path, mode_t mode)
    {
        int res = mkdir(path, mode);

        if(res == -1)
            return -errno;
        
        return 1;
    }

    int afs_FETCH(const std::string& path, char **buf, int *size)
    {
        FetchRequest request;
        request.set_path(path);


        FetchReply *reply = new FetchReply();

        ClientContext context;
        printf("before fetch\n");
        Status status = stub_->afs_FETCH(&context, request, reply);
        printf("after fetch\n");
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

    int afs_OPEN(const char *path, struct fuse_file_info *file_info, char cache_path[])
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

            char cbuf[] = "Check String";
            char nbuf[1000];

            char client_path[MAX_PATH_LENGTH];
            getLocalPath(path, cache_path, client_path);
            printf("path: %s\n", client_path);

            fd = open(client_path,   O_APPEND | O_RDWR);

            if(fd == -1) {
                printf("Open Return: %d\n", fd);

                rc = afs_FETCH(path, &buf, &size);
                if (rc<0) {
                    
                    return -ENOENT;
                }

                isFetched = 1;

                fd = creat(client_path, S_IRWXU);
                printf("new fd: %d\n", fd);
                if(fd==-1) {
                    printf("Create Error\n");
                    return -errno;
                }
                fd = open(client_path,  O_APPEND | O_RDWR);
                if(fd==-1) printf("Reopen Error\n"); 
            } else {

                lstat(client_path, &cacheFileInfo);
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
            file_info->fh = fd; 

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

    int afs_GETATTR(const char *path, struct stat *stats){
        ClientContext context;
        GetattrRes reply;
        GetattrReq request;

        request.set_path(path);

        Status status = stub_->afs_GETATTR(&context, request, &reply);
        if(reply.err() != 0){
            std::cout << " getattr errno: " << reply.err() << std::endl;
            return -1;
        }
        memset(stats, 0, sizeof(struct stat));

        stats->st_ino = reply.ino();
        stats->st_mode = reply.mode();
        stats->st_nlink = reply.nlink();
        stats->st_uid = reply.uid();
        stats->st_gid = reply.gid();
        stats->st_size = reply.size();
        stats->st_blksize = reply.blksize();
        stats->st_blocks = reply.blocks();
        stats->st_atime = reply.atime();
        stats->st_mtime = reply.mtime();
        stats->st_ctime = reply.ctime();
        return 0;
    }

    int afs_STORE(const std::string& path, char *buf, int size)
    {
        StoreReq request;
        request.set_path(path);
        request.set_size(size);
        request.set_buf(std::string(buf, size));

        StoreRes reply;

        ClientContext context;

        Status status = stub_->afs_STORE(&context, request, &reply);

        if (status.ok()) {
            return reply.error();
        } else {
            return -1;
        }
    }
    
    int afs_RELEASE(const char *path, struct fuse_file_info *fi)
    {
        int rc = 0;
        int isModified=1;
        char *buf;
        struct stat info;
        struct stat remoteFileInfo;

        fsync(fi->fh);

        memset(&info, 0, sizeof(struct stat));
        fstat(fi->fh, &info);
        afs_GETATTR(path, &remoteFileInfo);

        if(remoteFileInfo.st_mtime > info.st_mtime) {
            isModified = 0;
        }

        if(isModified) {
            buf = (char *)malloc(info.st_size);
            lseek(fi->fh, 0, SEEK_SET);
            read(fi->fh, buf, info.st_size);
            printf("To be sent: %s\n", buf);
            afs_STORE(path, buf, info.st_size);
            free(buf);
        }
        rc = close(fi->fh);

    /*
        char local_path[PATH_MAX];
        local_path[0] = '\0';
        char cacheFileName[80]; 
            snprintf(cacheFileName, 80, "%lu", hash((unsigned char *)path));

            strncat(local_path, cache_path, PATH_MAX);
            strncat(local_path, cacheFileName, PATH_MAX);
        lstat(local_path, &info);
        printf("After Close: %d\n", info.st_mtime); */
        return rc;
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
                      struct fuse_file_info *file_info, char cache_path[]){
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
            char local_path[MAX_PATH_LENGTH];
            getLocalPath(path, cache_path, client_path);

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