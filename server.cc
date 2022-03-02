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

char root_path[MAX_PATH_LENGTH];


class AfsServiceImplementation final : public AFS:: Service{
    Status afs_CREATE(
        ServerContext* context,
        const CreateReq* request,
        CreateRes* reply
    ) override { //returns a status by default
        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);
        int fd = open(path, O_CREAT, S_IRWXU | S_IRWXG); // fixing flags and modes for create
        if(fd == -1){
            reply->set_ack(-1);
        }
        else{
            reply->set_ack(1);
            close(fd);
        }
        return Status::OK;
    }

    Status afs_GETATTR(ServerContext* context, const GetattrReq* request, 
					 GetattrRes* reply) override {
        char path[MAX_PATH_LENGTH];
        getServerPath(request->path().c_str(), root_path, path);
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

    Status afs_LS(ServerContext* context,
                 const LsReq *request,
                 ServerWriter<LsRes>* writer) override {

      char path[MAX_PATH_LENGTH];
      getServerPath(request->path().c_str(), root_path, path);

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
