#include <grpc++/grpc++.h>
#include "nfsfuse.grpc.pb.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using grpc::ClientReader;


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