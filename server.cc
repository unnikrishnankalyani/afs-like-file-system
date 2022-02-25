#include <string>

#include <grpcpp/grpcpp.h>
#include "timetest.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using timetest::StrTest;
using timetest::StrRequest;
using timetest::StrReply;

class StrServiceImplementation final : public StrTest:: Service{
    Status sendRequest(
        ServerContext* context,
        const StrRequest* request,
        StrReply* reply
    ) override { //returns a status by default

        reply->set_ack(1);

        return Status::OK;
    }
};

void RunStrTestServer(std::string ipadd) {
    //create port on localhost 5000
    std::string address(ipadd+":5000");
    StrServiceImplementation service;

    ServerBuilder strServer; //server name

    strServer.AddListeningPort(address, grpc::InsecureServerCredentials());
    strServer.RegisterService(&service);

    std::unique_ptr<Server> server(strServer.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    server->Wait();
}


int main(int argc, char** argv) {
    std::string ipadd = "0.0.0.0";

    if (argc >2){
        ipadd = argv[2];
    }
    RunStrTestServer(ipadd);
    
    return 0;
}
