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
using afs::UnlinkReq;
using afs::UnlinkRes;
using afs::ChmodReq;
using afs::ChmodRes;
using afs::MkdirReq;
using afs::MkdirRes;
using afs::RmdirReq;
using afs::RmdirRes;

class AfsClient {
    public:
        AfsClient(std::shared_ptr<Channel> channel) : stub_(AFS::NewStub(channel)) {}

    int afs_CREATE(const char* path, char cache_path[],struct fuse_file_info *fi) {
        CreateReq request;

        request.set_path(path);

        CreateRes reply;

        ClientContext context;

        int fd;

        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);
    
        printf("path: %s\n", client_path);

        fd = open(client_path, O_CREAT | O_APPEND | O_RDWR, S_IRWXU | S_IRWXG); //changed last 3
        printf("Creating file in local cache\n");
        if (fd == -1) {
                printf("Create Error in local cache.. \n");
                return -errno;
        }

        Status status = stub_->afs_CREATE(&context, request, &reply);
        //Set file handler
        fi->fh = fd; 
        printf("**************** File handle CREATE ************: %d\n", fd);
        //add Retry
        if(status.ok()){
            return reply.ack();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -errno;
        }
        return 0;
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
            return -errno;
        }
    }

    int afs_OPEN(const char *path, struct fuse_file_info *file_info, char cache_path[])
    {
            char *buf;
            int size = -2;
            int rc;
            int fd;
            int fetchNewCopy = 0;

            struct stat cacheFileInfo;
            struct stat remoteFileInfo;

            char client_path[MAX_PATH_LENGTH];
            getLocalPath(path, cache_path, client_path);
            printf("path: %s\n", client_path);

            fd = open(client_path,  O_APPEND | O_RDWR, S_IRWXU | S_IRWXG); //changed last 3 removed O_CREAT because it has to fetch if not there
            
            if(fd == -1) {
                printf("1. file does not exist: %d\n", fd);
                fetchNewCopy = 1;
                printf("2. Open in CREAT mode (will be fetched): %d\n", fd);
                fd = open(client_path, O_CREAT |  O_APPEND | O_RDWR, S_IRWXU | S_IRWXG); //changed last 3

                if(fd==-1) printf("Reopen Error - Probable PERMISSION issues\n"); 

            } else {
                lstat(client_path, &cacheFileInfo);
                printf("4. Get stats to compare time stamps\n");
                afs_GETATTR(path, &remoteFileInfo); 
                if(remoteFileInfo.st_mtime > cacheFileInfo.st_mtime) {
                    fetchNewCopy = 1;
                    printf("5. Stale copy - fetch new \n");
                }
            }

            if(fetchNewCopy) {
                rc = ftruncate(fd, 0);
                printf("6. ftruncate: rc: %d\n", rc);
                if(rc<0) {
                    return -errno;
                }
                rc = afs_FETCH(path, &buf, &size);
                printf("8. Fetching new copy. rc: %d\n", rc);
                
                if (rc<0) {
                    return -ENOENT;
                }
                printf("9. Write to file: %s\n", buf);
                write(fd, buf, size);
                printf("10. fsync\n");
                fsync(fd);
                printf("9. File Size (as fetched from server and written into cache): %d\n", size);
            }
            //This is just for debug:
            lstat(client_path, &cacheFileInfo);
            printf("10. Final Size: %d\n", cacheFileInfo.st_size);
            
            //Set file handler
            file_info->fh = fd; 
            printf("**************** File handle OPEN ************: %d\n", fd);



        return 0;
    }

    int afs_READ(const char *path, char *buffer, size_t size, off_t offset,
		      struct fuse_file_info *file_info, char cache_path[]){
        
        int ret_code = 0;
        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);

        //Just to debug - 
        struct stat info;
        lstat(client_path, &info);
        printf("~~~~~~~BEFORE READ: Last Mod: %ld\n", info.st_mtime);

        printf("reading from : %s\n", client_path);
        printf("**************** File handle READ ************: %d\n", file_info->fh);
        printf("**************** File size, offset READ ************: %d, %d\n", size, offset);

        int fd = open(client_path, O_RDONLY);
        file_info->fh = fd;
        ret_code = pread(file_info->fh, buffer, size, offset);
        printf("READ buffer from : %s\n", buffer);


        //Just to debug - 
        lstat(client_path, &info);
        printf("~~~~~~~~AFTER READ: Last Mod: %ld\n", info.st_mtime);
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
            return -reply.err();
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

    int afs_TRUNCATE(const char *path, off_t size, char cache_path[])
    {
        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);
        printf("truncating: %s\n", client_path);
        int res = truncate(client_path, size);
        if(res == -1)
            return -errno;
        return 0;
    }

    int afs_CHOWN(const char *path, uid_t uid, gid_t gid, char cache_path[])
    {
        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);
        printf("changing ownership of: %s\n", client_path);
        int res = lchown(client_path, uid, gid);
        if(res == -1)
            return -errno;
        return 0;
    }

    int afs_UNLINK(const char *path, char cache_path[])
    {
        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);
        printf("unlinking: %s\n", client_path);
        int res = unlink(client_path);
        // if(res == -1)
        //     return -errno;
        // return 0;
        ClientContext context;
        UnlinkReq req;
        req.set_path(path);
        UnlinkRes reply;

        Status status = stub_->afs_UNLINK(&context, req, &reply);
        if (status.ok()) {
            return 0;
        } else {
            return -reply.error();
        }
    }

    int afs_UTIMENS(const char *path, const struct timespec ts[2], char cache_path[])
    {
        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);
        printf("utimens: %s\n", client_path);
        int res = utimensat(0, client_path, ts, AT_SYMLINK_NOFOLLOW);
        if(res == -1)
            return -errno;
        return 0;
    }

    int afs_CHMOD(const char *path, mode_t mode, char cache_path[])
    {
        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);
        printf("changing permissions of: %s\n", client_path);
        int res = chmod(client_path, mode);
        // if(res == -1)
        //     return -errno;
        // return 0;
        ClientContext context;
        ChmodReq req;
        req.set_path(path);
        req.set_mode(mode);
        ChmodRes reply;

        Status status = stub_->afs_CHMOD(&context, req, &reply);
        if (status.ok()) {
            return 0;
        } else {
            return -reply.error();
        }
    }
    

    int afs_MKDIR(const char *path, mode_t mode, char cache_path[])
    {
        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);
        printf("creating directory: %s\n", client_path);
        int res = mkdir(client_path, mode);
        // if(res == -1)
        //     return -errno;
        // return 0;
        ClientContext context;
        MkdirReq req;
        req.set_path(path);
        req.set_mode(mode);
        MkdirRes reply;

        Status status = stub_->afs_MKDIR(&context, req, &reply);
        if (status.ok()) {
            return 0;
        } else {
            return -reply.error();
        }
    }

    int afs_RMDIR(const char *path, char cache_path[])
    {
        char client_path[MAX_PATH_LENGTH];
        getLocalPath(path, cache_path, client_path);
        printf("removing directory: %s\n", client_path);
        int res = rmdir(client_path);
        // if(res == -1)
        //     return -errno;
        // return 0;
        ClientContext context;
        RmdirReq req;
        req.set_path(path);
        RmdirRes reply;

        Status status = stub_->afs_RMDIR(&context, req, &reply);
        if (status.ok()) {
            return 0;
        } else {
            return -reply.error();
        }
    }

    int afs_STORE(const std::string& path, char *buf, int size)
    {
        StoreReq request;
        request.set_path(path);
        request.set_size(size);
        printf("Store request: Path = %s Size = %d \n", path, size);
        request.set_buf(std::string(buf, size));

        StoreRes reply;

        ClientContext context;

        Status status = stub_->afs_STORE(&context, request, &reply);

        if (status.ok()) {
            return reply.error();
        } else {
            return -reply.error();
        }
    }
    
    int afs_RELEASE(const char *path, struct fuse_file_info *fi)
    {
        int rc = 0;
        int isModified=1;
        char *buf;
        struct stat info;
        struct stat remoteFileInfo;
        
        //Just to debug - 
        fstat(fi->fh, &info);
        printf("RELEASE: LOCAL: Last Mod before fsync: %ld\n", info.st_mtime);

        // fsync(fi->fh);

        memset(&info, 0, sizeof(struct stat));
        fstat(fi->fh, &info);
        afs_GETATTR(path, &remoteFileInfo);

        //Just to debug - 
        printf("RELEASE: LOCAL: Last Mod: after fsync %ld\n", info.st_mtime);
        printf("RELEASE: REMOTE: Last Mod: %ld\n", remoteFileInfo.st_mtime);

        if(remoteFileInfo.st_mtime >= info.st_mtime) {
            isModified = 0;
        }

        if(isModified) {
            fsync(fi->fh);
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

    // int afs_FLUSH(const char *path, struct fuse_file_info *fi)
    // {
    //     int res = close(dup(fi->fh));
    //      if(res == -1)
    //         return -errno;
    //     return 0;
    // }

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
            return -errno;
        }
}



    int afs_FSYNC(const char *path, int isdatasync, struct fuse_file_info *fi)
    {
        int res;
        // char client_path[MAX_PATH_LENGTH];
        // getLocalPath(path, cache_path, client_path);
        // printf("changing permissions of: %s\n", client_path);
        if (isdatasync)
            res = fdatasync(fi->fh);
        else
            res = fsync(fi->fh);
        if (res == -1)
            return -errno;

        return 0;
    }

    // int afs_GETXATTR(const char *path, const char *name, char *value,
	// 		size_t size)
    // {
    //     int res = lgetxattr(path, name, value, size);
    //     if (res == -1)
    //         return -errno;
    //     return res;
    // }


    int afs_WRITE(const char *path, const char *buffer, size_t size, off_t offset,
                      struct fuse_file_info *file_info, char cache_path[]){
        int ret_code = 0;
        struct stat info;
        fstat(file_info->fh, &info);
        printf("~~~~~~~~BEFORE WRITE: Last Mod: %ld\n", info.st_mtime);

        ret_code = write(file_info->fh, buffer, size);
        fsync(file_info->fh);
        fstat(file_info->fh, &info);
        printf("~~~~~~~~AFTER WRITE and FSYNC: Last Mod: %ld\n", info.st_mtime);

        if(ret_code < 0) {
            printf("Error while writing into file: %d\n", errno);
            int fd;
            char local_path[MAX_PATH_LENGTH];
            getLocalPath(path, cache_path, local_path);

            fd = open(local_path,  O_APPEND | O_RDWR | S_IRWXU | S_IRWXG | S_IRWXO); //changed last 3
            printf("writing to file: %s\n", local_path);
            printf("size: %zu\n", size);
            printf("offset: %jd\n", (intmax_t)size);
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