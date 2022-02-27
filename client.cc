#include <string>
#include <time.h>
#include <sys/time.h>
#include <grpcpp/grpcpp.h>
#include "afs.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using afs::AFS;
using afs::CreateReq;
using afs::CreateRes;

class AfsClient {
    public:
        AfsClient(std::shared_ptr<Channel> channel) : stub_(AFS::NewStub(channel)) {}

    int afs_CREATE(grpc::string a) {
        CreateReq request;

        request.set_hi(a);

        CreateRes reply;

        ClientContext context;

        Status status = stub_->afs_CREATE(&context, request, &reply);

        if(status.ok()){
            return reply.ack();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }

    private:
        std::unique_ptr<AFS::Stub> stub_;
};

void RunAfsClient(std::string ipadd) {
    std::string address(ipadd+":5000");
    AfsClient client(
        grpc::CreateChannel(
            address,
            grpc::InsecureChannelCredentials()
        )
    );

    int response;

    std::string msg = "testmsg: hi!";
    response = client.afs_CREATE(msg);
            
    
    std::cout << "Success: " << response << std::endl;
}

int main(int argc, char* argv[]){

    std::string ipadd = "0.0.0.0";
    if (argc >1){
            ipadd = argv[1];
        }

    RunAfsClient(ipadd);

    return 0;
}
