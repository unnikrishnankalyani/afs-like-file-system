#include <string>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <iostream>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <grpcpp/grpcpp.h>
#include "afs.grpc.pb.h"
#include "commonheaders.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

using afs::AFS;
using afs::CreateReq;
using afs::CreateRes;
using afs::LsReq;
using afs::LsRes;
using afs::GetattrReq;
using afs::GetattrRes;
using afs::FetchRequest;
using afs::FetchReply;
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

char root_path[MAX_PATH_LENGTH];


class AfsServiceImplementation final : public AFS:: Service{
    Status afs_CREATE(
        ServerContext* context,
        const CreateReq* request,
        CreateRes* reply
    ) override { //returns a status by default
        std::cout << "server create" <<std::endl;
        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);
        int fd = open(path, O_CREAT, S_IRWXU | S_IRWXG); // fixing flags and modes for create. changed last 1
        
        if(fd == -1){
            perror(strerror(errno));
            reply->set_ack(errno);
        }
        else{
            reply->set_ack(0);
            close(fd);
        }
        return Status::OK;
    }

      Status afs_STORE(ServerContext* context, const StoreReq* request,
               StoreRes* reply) override {

        int fd;

        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);

        printf("AFS PATH STORE: %s\n", path);

        fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG);

        if(fd == -1) {
            perror(strerror(errno));
		    reply->set_error(errno);
            return Status::OK;
        }

        printf("Received String: %s\n", (request->buf()).c_str());
        printf("Size: %d\n", request->size());
        write(fd, (request->buf()).data(), request->size());
        close(fd);

        reply->set_error(0);
        return Status::OK;
    }



    Status afs_GETATTR(ServerContext* context, const GetattrReq* request, 
					 GetattrRes* reply) override {
        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);
        printf("AFS server PATH, GETATTR: %s\n", path);

        struct stat stats;
		int res = lstat(path, &stats);

        if(res == -1){
		    perror(strerror(errno));
		    reply->set_err(errno);
		}
		else{
            reply->set_ino(stats.st_ino);
            reply->set_mode(stats.st_mode);
            reply->set_nlink(stats.st_nlink);
            reply->set_uid(stats.st_uid);
            reply->set_gid(stats.st_gid);

            reply->set_size(stats.st_size);
            reply->set_blksize(stats.st_blksize);
            reply->set_blocks(stats.st_blocks);
            reply->set_atime(stats.st_atime);
            reply->set_mtime(stats.st_mtime);
            reply->set_ctime(stats.st_ctime);
			
		    reply->set_err(0);
		}
		
        return Status::OK;
	
	}

    Status afs_UNLINK(ServerContext* context, const UnlinkReq* request, 
					 UnlinkRes* reply) override {
        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);
        printf("AFS server PATH, unlink: %s\n", path);

        int res = unlink(path);
        if(res == -1)
        { 
            perror(strerror(errno));
            reply->set_error(errno);
        }
        return Status::OK;
	
	}

    Status afs_CHMOD(ServerContext* context, const ChmodReq* request, 
					 ChmodRes* reply) override {
        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);
        printf("AFS server PATH, Chmod: %s\n", path);

        int res = chmod(path, request->mode());
        if(res == -1)
        { 
            perror(strerror(errno));
            reply->set_error(errno);
        }
        return Status::OK;
	
	}

    Status afs_MKDIR(ServerContext* context, const MkdirReq* request, 
					 MkdirRes* reply) override {
        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);
        printf("AFS server PATH, mkdir: %s\n", path);

        int res = mkdir(path, request->mode());
        if(res == -1)
        { 
            perror(strerror(errno));
            reply->set_error(errno);
            return Status::CANCELLED;
        }
        return Status::OK;
	
	}

    Status afs_RMDIR(ServerContext* context, const RmdirReq* request, 
					 RmdirRes* reply) override {
        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);
        printf("AFS server PATH, rmdir: %s\n", path);

        int res = rmdir(path);
        if(res == -1)
        { 
            perror(strerror(errno));
            reply->set_error(errno);
        }
        return Status::OK;
	
	}

    Status afs_FETCH(ServerContext* context, const FetchRequest* request,
               FetchReply* reply) override {

        int fd;
        struct stat info;

        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);

        printf("AFS server PATH FETCH: %s\n", path);

        fd = open(path, O_RDWR);

        if(fd == -1) {
            reply->set_error(-1);
            return Status::OK;
        }

        fstat(fd, &info);

        char *buf = (char *)malloc(info.st_size);

        lseek(fd, 0, SEEK_SET);
        read(fd, buf, info.st_size);
        close(fd);

        printf("Read string: %s\n", buf);

        reply->set_error(0);
        reply->set_buf(std::string(buf,info.st_size));
        reply->set_size(info.st_size);
        return Status::OK;
    
    }

    Status afs_LS(ServerContext* context,
                 const LsReq *request,
                 ServerWriter<LsRes>* writer) override {

      char path[MAX_PATH_LENGTH];
      getServerPath(request->path().c_str(), root_path, path);
      printf("AFS server PATH, LS: %s\n", path);
      DIR *dp;
      struct dirent *de;
      LsRes reply;

      dp = opendir(path);
      if (dp == NULL) {
          reply.set_error(-1);
          writer->Write(reply);
          return Status::OK;
      }

      de = readdir(dp);
      if (de == 0) {
          reply.set_error(-1);
          writer->Write(reply);
          return Status::OK;
      }


      do {
          reply.set_error(0);
          reply.set_name(std::string(de->d_name));
          writer->Write(reply);
      } while ((de = readdir(dp)) != NULL);

      return Status::OK;
  }
};

void RunAfsServer(std::string ipadd) {
    //create port on localhost 5000
    std::string address("0.0.0.0:50051");
    AfsServiceImplementation service;
    std::cout << "1" << std::endl;
    ServerBuilder afsServer; //server name
    std::cout << "2" << std::endl;
    afsServer.AddListeningPort(address, grpc::InsecureServerCredentials());
    std::cout << "3" << std::endl;
    afsServer.RegisterService(&service);
    std::cout << "4" << std::endl;
    std::unique_ptr<Server> server(afsServer.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    server->Wait();
}


int main(int argc, char** argv) {
    std::string ipadd = "0.0.0.0";

    strncpy(root_path, argv[1], MAX_PATH_LENGTH);
    std::cout << "afs path : " << root_path << std::endl;
    RunAfsServer(ipadd);
    
    return 0;
}
