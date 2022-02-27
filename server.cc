#include <string>

#include <grpcpp/grpcpp.h>
#include "afs.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using afs::AFS;
using afs::CreateReq;
using afs::CreateRes;

class AfsServiceImplementation final : public AFS:: Service{
    Status afs_CREATE(
        ServerContext* context,
        const CreateReq* request,
        CreateRes* reply
    ) override { //returns a status by default

        reply->set_ack(1);

        return Status::OK;
    }
};

void RunAfsServer(std::string ipadd) {
    //create port on localhost 5000
    std::string address(ipadd+":5000");
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
