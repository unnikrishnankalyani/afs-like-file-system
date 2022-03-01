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
using grpc::Status;

using afs::AFS;
using afs::CreateReq;
using afs::CreateRes;
using afs::LsReq;
using afs::LsRes;


class AfsServiceImplementation final : public AFS:: Service{
    Status afs_CREATE(
        ServerContext* context,
        const CreateReq* request,
        CreateRes* reply
    ) override { //returns a status by default
        char* path;
        strcat(path, request->path().c_str());
        path[strlen(path)] = '\0';
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

    Status ListDir(ServerContext* context,
                 const LsReq *request,
                 ServerWriter<LsRes>* writer) override {

      char path[PATH_MAX];
      path[0] = '\0';

      strncat(path, afs_path, PATH_MAX);
      strncat(path, (request->path()).c_str(), PATH_MAX);

      DIR *dp;
      struct dirent *de;
      LsRes reply;

      dp = opendir(path);
      if (dp == NULL) {
          reply.set_ack(-1);
          writer->Write(reply);
          return Status::OK;
      }

      de = readdir(dp);
      if (de == 0) {
          reply.set_ack(-1);
          writer->Write(reply);
          return Status::OK;
      }


      do {
          reply.set_ack(1);
          reply.set_filename(std::string(de->d_name));
          writer->Write(reply);
      } while ((de = readdir(dp)) != NULL);

      return Status::OK;
  }

};

void RunAfsServer(std::string ipadd) {
    //create port on localhost 5000
    std::string address(ipadd+":50051");
    AfsServiceImplementation service;

    ServerBuilder afsServer; //server name

    afsServer.AddListeningPort(address, grpc::InsecureServerCredentials());
    afsServer.RegisterService(&service);

    std::unique_ptr<Server> server(afsServer.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    server->Wait();
}


int main(int argc, char** argv) {
    std::string ipadd = "0.0.0.0";

    RunAfsServer(ipadd);
    
    return 0;
}
